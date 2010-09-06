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

#include "evfilter.h"

static struct evf_io_queue *queue;

static void input_commit(struct input_event *ev, void *data)
{
	int uinput_fd = (int) data;
	
	write(uinput_fd, ev, sizeof (struct input_event));
	printf("Event!\n");
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

static void device_plugged(const char *dev)
{
	int fd;
	struct uinput_user_dev dev_info;
	struct evf_line *line;
	union evf_err err;

	fprintf(stderr, "Trying to create input line for %s.\n", dev);
	/*
	 * Create new input device for the other end of input line.
	 */
	//TODO: do this correctly
	memset(&dev_info, 0, sizeof (dev));
	strcpy(dev_info.name, "test");

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
	line = evf_line_create(dev, input_commit, (void*)fd, 0, &err);

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

int main(int argc, char *argv[])
{
	int fd;

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

	for (;;)
		evf_io_queue_wait(queue, NULL);

	return 0;
}
