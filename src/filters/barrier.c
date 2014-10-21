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
 * Evfilter barrier:
 *
 * holds events until sync event
 *
 * parameters:
 *
 * history integer
 *
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_msg.h"

struct barrier {
	unsigned int queue_index;
	unsigned int queue_size;
	struct input_event queue[];
};

/*
 * Flush all events in queue
 */
static void flush_queue(struct evf_filter *self, struct barrier *barrier)
{
	unsigned int i;

	for (i = 0; i < barrier->queue_index; i++)
		self->next->modify(self->next, &barrier->queue[i]);

	barrier->queue_index = 0;
}

/*
 * Copy event to queue
 */
static void insert_queue(struct barrier *barrier, struct input_event *ev)
{
	/*
	 * Queue is full, drop event
	 */
	if (barrier->queue_index >= barrier->queue_size)
		return;

	barrier->queue[barrier->queue_index++] = *ev;
}

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct barrier *barrier = (struct barrier*) self->data;

	/*
	 * Flush queue, don't forget to send sync event
	 */
	if (ev->type == EV_SYN && ev->code == SYN_REPORT) {
		flush_queue(self, barrier);
		self->next->modify(self->next, ev);
	} else
		insert_queue(barrier, ev);
}

struct evf_filter *evf_barrier_alloc(unsigned int history)
{
	struct evf_filter *evf = malloc(sizeof(struct evf_filter) + sizeof(struct barrier) + history*sizeof(struct input_event));
	struct barrier *tmp;

	evf_msg(EVF_DEBUG, "Creating barrier filter");

	if (evf == NULL)	{
		evf_msg(EVF_ERR, "Allocating error");
		return NULL;
	}

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Barrier";
	evf->desc   = "Creates barrier so every group of events came at once.";

	tmp = (struct barrier*) evf->data;
	tmp->queue_size  = history;
	tmp->queue_index = 0;

	evf_msg(EVF_DEBUG, "Barrier filter created");

	return evf;
}

static struct evf_lim_int history_limit = { 0, 100 };

static struct evf_param barrier_params[] = {
	{ "history", evf_int, &history_limit, "size of the queue", NULL },
};

struct evf_filter *evf_barrier_creat(char *params, union evf_err *err)
{
	int history;
	struct evf_filter *evf;

	if (evf_load_params(err, params, barrier_params, &history) == -1)
		return NULL;
	
	evf = evf_barrier_alloc(history);
	
	if (evf == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
