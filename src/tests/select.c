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
 * Copyright (C) 2008-2009 Cyril Hrubis                                       *
 *                                                                            *
 ******************************************************************************/

/*

  This program is testing select queue implementation by opening
  /dev/input/eventX passed as arguments and then printing events from these
  devices as they are turning up.

 */

#include <stdio.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "evfilter.h"
#include "linux_input.h"

static int event_read(struct evf_select_memb *self)
{
	struct input_event ev;

	if (read(self->fd, &ev, sizeof(struct input_event)) < 0) {
		printf("Removing fd %i from queue.\n", self->fd);
		return EVF_SEL_REM | EVF_SEL_CLOSE;
	}

	printf("Event from fd %i:\n", self->fd);
	evf_input_print(stdout, " *** ", &ev);
	printf("\n");

	return EVF_SEL_OK;
}

int main(int argc, char *argv[])
{
	struct evf_select_queue *queue = evf_select_new();
	int i;
	int fd;
	char name[256];

	if (argc < 2) {
		fprintf(stderr, "usage: ./select /dev/input/eventX /dev/input/eventY ...\n");
		return -1;
	}

	if (queue == NULL) {
		fprintf(stderr, "Can't allocate select queue.\n");	
		return -1;
	}

	for (i = 1; i < argc; i++) {
		
		printf("Opening %s ... ", argv[i]);

		fd = open(argv[i], O_RDONLY);
		
		if (fd < 0) {
			printf("Err\n - %s\n", strerror(errno));
			continue;
		}
		
		if (evf_input_get_name(fd, name, 256) < 0) {
			printf("Err\n - is not an input device.\n");
			close(fd);
			continue;
		}

		printf("Ok\nDevice name: %s\n", name);

		if (!evf_select_add(queue, fd, event_read, NULL)) {
			printf("Can't add filedescriptor into queue, malloc failed :(\n");
			close(fd);
		}
	}

	if (!evf_select_count(queue)) {
		fprintf(stderr, "No filed descriptors in queue, exiting...\n");
		return 0;
	}

	for (;;)
		evf_select(queue, NULL);

	return 0;
}
