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
  
  Evfilter line is abstraction for linked list of filters, that are used to
  filter events, along with input device path and file descriptor. So evfilter
  line represents one input device with filters and is usable to be used as
  high level logic interface to this library.

 */

#ifndef __EVF_LINE_H__
#define __EVF_LINE_H__

union evf_err;
struct evf_filter;
struct input_event;

/*
 * Filter line
 */
struct evf_line {
	struct evf_filter *begin;
	struct evf_filter *end;
	/* file descriptor */
	int fd;
	/* path to the input device */
	char input_device[];
};

/*
 * Process filter line with events that are waiting to be read from fd.  Call
 * this after select says there are data, or as polling in non blocking mode.
 *
 * 0 is returned on succes.
 *
 * -1 on failure to read from fd (EAGAIN is not threated as error and 0 is
 *  reported).
 */
int evf_line_process(struct evf_line *line);

/*
 * Allocate space for input line, inserts commit filter and additionally
 * barrier filter, if variable use_barriers is non zero, filter barriers with
 * history = use_barriers is attached before commit filter.
 * 
 * With even_empty == 0 line with just commit and possibly barrier filters
 * would be created when no additional filters are found in system
 * configuration.
 */
struct evf_line *evf_line_create(const char *input_device,
                                 void (*commit)(struct input_event *ev,
				                void *priv),
				 void *priv, unsigned int use_barriers,
				 union evf_err *err, int even_empty);

/*
 * Attach filter at the end of the line, but before commit and barrier.
 */
void evf_line_attach_filter(struct evf_line *line, struct evf_filter *filter);

/* 
 * Send event to linked list of filters. After calling this function
 * you can get from zero to infinite events through function passed
 * to the filter_commit.
 */
void evf_line_process_event(struct evf_filter *root, struct input_event *ev);

/*
 * Free all memory allocated in filter line. Also close file descriptor
 * associated with line. Returns pointer to void *data.
 */
void *evf_line_destroy(struct evf_line *line);

/*
 * Print filter names from filter line to stdout, use for debugging purpuses
 * only.
 */
void evf_line_print(struct evf_line *line);

/*
 * Returns file descriptor for line.
 */
int evf_line_fd(struct evf_line *line);

#endif /* __EVF_LINE_H__ */
