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
 * This is example how to use evfilter line.
 */

#include <unistd.h>
#include <stdio.h>
#include "evfilter.h"

static void commit(struct input_event *ev, void *data)
{
	(void) ev;
	(void) data;
}

int main(int argc, char *argv[])
{
	struct evf_line   *line[100];
	struct evf_filter *filter;
	union  evf_err     err;
	int i, line_idx = 0;
	char params[] = "prefix = '' file=stdout";

	if (argc < 2) {
		fprintf(stderr, "usage: ./input_line /dev/input/eventX /dev/input/eventY ....\n");
		return 1;
	}

	if (argc > 101) {
		fprintf(stderr, "too much inputs\n");
		return 1;
	}

	for (i = 1; i < argc; i++) {
		
		printf("Opening %s ... ", argv[i]);
		
		line[line_idx] = evf_line_create(argv[i], commit, NULL, 20, &err);
		
		if (line[line_idx] == NULL) {
			printf("Error creating evf_line\n");
			evf_err_print(&err);
			return 1;
		} else {
			printf("Ok\n");
			line_idx++;
		}
	}

	filter = evf_filter_load("dump", params, &err);

	if (filter != NULL)
		evf_line_attach_filter(line[0], filter);
	else
		evf_err_print(&err);

	for (i = 0; i < line_idx; i++) {
		evf_line_print(line[i]);
		printf("\n");
	}

	/*
	 * Poll all lines and sleep
	 */
	for (;;) {
		for (i = 0; i < line_idx; i++)
			evf_line_process(line[i]);
		
		usleep(1000);
	}

	return 0;
}
