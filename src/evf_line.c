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

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <stdio.h>

#include "evf_line.h"

#include "filters/filters.h"
#include "evf_filter.h"
#include "evf_err.h"
#include "evf_profile.h"
#include "linux_input.h"

#include "evf_struct.h"

/*
 * Read events from file descriptor and pass them to filter line
 */
int evf_line_process(struct evf_line *line)
{
	struct input_event ev;

	/*
	 * Read can teoretically throw sigpipe, but here it IMHO shouldn't
	 * happend unless kernel has crashed.
	 */
	if (read(line->fd, &ev, sizeof (struct input_event)) < 0) {
		if (errno != EAGAIN)
			return -1;
	}

	evf_line_process_event(line->begin, &ev);

	return 0;
}

/*
 * Opens input device, tries ioctl.
 */
static int open_input_device(const char *input_device, union evf_err *err)
{
	int fd, version;

	/* open input device */
	fd = open(input_device, O_RDONLY);

	if (fd < 0) {
		//TODO: context
		err->type = evf_errno;
		err->err_no.err_no = errno;
	}

	/* doesn't understand ioctl => not an input device */
	if (evf_input_get_version(fd, &version)) {
		close(fd);
		//TODO: fill evf_err
	}

	return fd;
}


/*
 * Create input line
 */
struct evf_line *evf_line_create(const char *input_device,
                                 void (*commit)(struct input_event *ev,
				                void *priv),
				 void *priv, unsigned int use_barriers,
				 union evf_err *err)
{
	struct evf_line   *line;
	struct evf_filter *fcommit;
	struct evf_filter *fbarrier = NULL;	
	int fd;

	fd = open_input_device(input_device, err);

	if (fd < 0)
		return NULL;

	/* let's the allocation begins ;) */
	line = malloc(sizeof (struct evf_line) + strlen(input_device) + 1);

	if (line == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
		close(fd);
		return NULL;
	}

	/* load system profiles */
	line->begin = evf_load_system_profile(fd, err);
	line->end   = evf_filters_last(line->begin);
	
	/* err is filled from evf_load_system_profile */
	if (err->type != evf_ok) {
		free(line);
		close(fd);
		return NULL;
	}

	fcommit = evf_commit_alloc(commit, priv);

	if (use_barriers)
		fbarrier = evf_barrier_alloc(use_barriers);
		
	if (fcommit == NULL || (use_barriers && fbarrier == NULL)) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
		free(fcommit);
		free(fbarrier);
		evf_filters_free(line->begin);
		free(line);
		close(fd);
		return NULL;
	}
	
	/* build the structure in memory */
	line->fd = fd;
	
	strcpy(line->input_device, input_device);

	if (use_barriers)
		fbarrier->next = fcommit;
	else 
		fbarrier       = fcommit;

	if (line->end == NULL) {
		line->begin = fbarrier;
		line->end   = fbarrier;
	} else
		line->end->next = fbarrier;
	
	return line;
}

/*
 * Inserts filter at the end of linked list of filters, but before commit and barrier.
 */
void evf_line_attach_filter(struct evf_line *line, struct evf_filter *filter)
{
	
	if (line->end == line->begin) {
		filter->next    = line->end;
		line->begin     = filter;
		line->end       = filter;
	} else {
		filter->next    = line->end->next;
		line->end->next = filter;
	}
}


/*
 * Start processing for event ev
 */
void evf_line_process_event(struct evf_filter *root, struct input_event *ev)
{
	root->modify(root, ev);
}

/*
 * Free all evfilters in line
 */
void *evf_line_free(struct evf_line *line)
{
	void *priv;

	priv = evf_filters_free(line->begin);
	close(line->fd);
	free(line);

	return priv;
}

int evf_line_fd(struct evf_line *line)
{
	return line->fd;
}

/*
 * Debug function, prints all filters in line to stdout.
 */
void evf_line_print(struct evf_line *line)
{
	char name[256];

	evf_input_get_name(line->fd, name, 256);

	printf("Filter line for input %s (%s)\n ", line->input_device, name);

	evf_filters_print(line->begin);
}
