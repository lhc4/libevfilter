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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "evfilter.h"
#include "evf_input.h"

static void commit(struct input_event *ev, void *data)
{
	printf("%s:\n",  (char*)data);
        evf_input_print(stdout, "", ev);
	printf("\n");
}

static int event_read(struct evf_io_queue_memb *self)
{
	evf_line_process(self->priv);

	return EVF_IO_QUEUE_OK;
}

int main(int argc, char *argv[])
{
	struct evf_io_queue *queue = evf_io_queue_new();
	struct evf_line *line;
	union evf_err err;
	int i;

	if (argc < 2) {
		fprintf(stderr, "usage: ./%s /dev/input/eventX /dev/input/eventY ...\n", argv[0]);
		return -1;
	}

	if (queue == NULL) {
		fprintf(stderr, "Can't allocate io queue.\n");	
		return -1;
	}

	for (i = 1; i < argc; i++) {
		
		printf("Opening %s ... ", argv[i]);

		line = evf_line_create(argv[i], commit, argv[i], 6, &err, 1);
		
		if (line == NULL) {
			evf_err_print(&err);
			continue;
		}

		if (!evf_io_queue_add(queue, evf_line_fd(line), event_read, line)) {
			printf("Can't add filedescriptor into queue, malloc failed :(\n");
			evf_line_destroy(line);
		} else {
			printf("ok\n\n");
			evf_line_print(line);
			printf("\n");
		}
	}

	if (evf_io_queue_get_count(queue) == 0) {
		fprintf(stderr, "No filed descriptors in queue, exiting...\n");
		return 0;
	}

	for (;;)
		evf_io_queue_wait(queue, NULL);

	return 0;
}
