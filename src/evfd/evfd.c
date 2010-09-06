/******************************************************************************
 * This file is part of evfilter library.                                     *
 *                                                                            *
 * Evfilter library is free software; you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * Evfilter library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with evfilter library; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 *                                                                            *
 * Copyright (C) 2008-2010 Cyril Hrubis <metan@ucw.cz>                        *
 *                                                                            *
 ******************************************************************************/

/*
  
  Evfilter daemon. Run filters on input devices accordingly to evfilter
  configuration.

 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "evfilter.h"

static struct evf_io_queue *queue;

static void input_commit(struct input_event *ev, void *data)
{
	int uinput_fd = (int) data;
	
	write(uinput_fd, ev, sizeof (struct input_event));
}

/*
 * Data on input line, process them (may call line_commit callback).
 */
static int line_data(struct evf_io_queue_memb *self)
{
	//TODO: return value from evf_line_process()
	evf_line_process(self->priv);
	
	return EVF_IO_QUEUE_OK;
}

/*
 * Returns 1 if device is created by evfd.
 */
static int our_input_device(const char *dev)
{
	int fd = open(dev, O_RDONLY);
	char name[128];

	if (fd < 0)
		return 1;

	evf_input_get_name(fd, name, 128);

	close(fd);

	if (!strcmp(name, "uinput device"))
		return 1;

	return 0;
}

static void device_plugged(const char *dev)
{
	int fd, ret;
	struct uinput_user_dev dev_info;
	struct evf_line *line;
	union evf_err err;

	if (our_input_device(dev))
		return;

	fprintf(stderr, "Trying to create input line for %s.\n", dev);
	/*
	 * Create new input device for the other end of input line.
	 */
	//TODO: do this correctly
	memset(&dev_info, 0, sizeof (dev));
	strcpy(dev_info.name, "uinput device");

	fd = evf_uinput_create(&dev_info);

	if (fd < 0) {
		fprintf(stderr, "evf_uinput_create() failed\n"
		                "Do you have kernel capable of uinput and "
				"rights to write /dev/input/uinput?\n");
		return;
	}

	/* 
	 * Create input line (accordingly to evfilter configuration)
	 * without barrier filter.
	 *
	 * This should load all filters specified by evfilter system
	 * configuration into evfilter input line.
	 */
	//TODO: hack
	line = evf_line_create(dev, input_commit, (void*)fd, 0, &err, 0);

	if (line == NULL) {
		/* no filter configured for this inpud device */
		if (err.type == evf_ok) {
			fprintf(stderr, "No evfilter configuration found.\n");
			evf_uinput_destroy(fd);
			return;
		} else {
			evf_err_print(&err);
			evf_uinput_destroy(fd);
			return;
		}
	}

	/* we have line add it into the queue */
	if (!evf_io_queue_add(queue, evf_line_fd(line), line_data, line)) {
		fprintf(stderr, "Failed to add input line (%s) to io queue.\n",
		        dev);
		evf_uinput_destroy(fd);
		evf_line_destroy(line);
		return;
	}

	if ((ret = evf_input_grab(evf_line_fd(line))) != 0)
		fprintf(stderr, "Failed to grab device %i.\n", ret);

	fprintf(stderr, "Evfilter line for %s has been created.\n", dev);
}

/*
 * Data on hotplug fd => generated hotplug events (calls device_plugged callback).
 */
static int hotplug_data(struct evf_io_queue_memb *self
                        __attribute__ ((unused)))
{
	//TODO: errors from evf_hotplug_rescan()
	evf_hotplug_rescan();

	return EVF_IO_QUEUE_OK;
}

static int looping = 1;

static void sighandler(int sig __attribute__ ((unused)))
{
	looping = 0;
}

int main(int argc, char *argv[])
{
	int fd;
	struct evf_io_queue_memb *i;
	
	/* register handler */
	signal(SIGQUIT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGINT, sighandler);

	/* create io queue for all fds */
	queue = evf_io_queue_new();

	if (queue == NULL) {
		fprintf(stderr, "Can't allocate io queue.\n");
		return 1;
	}

	/* initalize hotplug input device watching */
	if ((fd = evf_hotplug_init(device_plugged, NULL)) < 0) {
		fprintf(stderr, "Can't initalize hotplug: %s.\n",
		        strerror(errno));
		evf_io_queue_destroy(queue, 0);
		return 1;
	}
	
	/* create hotplug handler */
	if (!evf_io_queue_add(queue, fd, hotplug_data, NULL)) {
		fprintf(stderr, "Can't allocate hotplug queue handler.\n");
		evf_io_queue_destroy(queue, 0);
		return 1;
	}

	while (looping)
		evf_io_queue_wait(queue, NULL);

	fprintf(stderr, "Got signal, exitting ...\n");

	/* cleanup */	
	EVF_IO_QUEUE_MEMB_LOOP(queue, i) {
		if (i->priv != NULL) {
			struct evf_line *line = i->priv;
			int fd;
			/* ungrab device */
			evf_input_ungrab(line->fd);
			/* destroy evf line and get commit priv pointer */
			fd = (int) evf_line_destroy(line);
			evf_uinput_destroy(fd);	
		}
	}

	return 0;
}
