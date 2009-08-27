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
 * along with Evfilter library; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 *                                                                            *
 * Copyright (C) 2008-2009 Cyril Hrubis                                       *
 *                                                                            *
 ******************************************************************************/

/*

  This program is testing select/hotplug interface by printing out every event
  from every input device plugged into computer.

 */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "evfilter.h"
#include "linux_input.h"

static struct evf_select_queue *queue;

static int read_event(struct evf_select_memb *self)
{
	struct input_event ev;

	if (read(self->fd, &ev, sizeof(struct input_event)) < 0) {
		printf("read failed, closing fd %i\n", self->fd);
		return EVF_SEL_REM | EVF_SEL_CLOSE;
	}
	
	printf("event from fd %i\n", self->fd);

	evf_input_print(stdout, " *** ", &ev);

	return EVF_SEL_OK;
}

static void device_plugged(const char *dev)
{
	int fd;

	printf("device %s plugged.\n", dev);

	fd = open(dev, O_RDONLY);

	if (fd < 0) {
		perror("Can't open device");
		return;
	}

	evf_select_add(queue, fd, read_event, NULL);
}

static int hotplug_rescan(struct evf_select_memb *self)
{
	(void) self;

	evf_hotplug_rescan();
	
	return 0;
}

int main(void)
{
	int fd;
	queue = evf_select_new();

	if (queue == NULL) {
		fprintf(stderr, "Can't allocate queue for select!\n");
		exit(EXIT_FAILURE);
	}

	if ((fd = evf_hotplug_init(device_plugged, NULL)) < 0) {
		fprintf(stderr, "Can't initalize hotplug: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* add hotplug into queue */
	evf_select_add(queue, fd, hotplug_rescan, NULL);

	printf("select!\n");

	for (;;)
		evf_select(queue);
	
	return 0;
}

