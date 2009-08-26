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
 *
 * Evfilter commit:
 *
 * Sends events back to the higher layer.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <linux/input.h>

#include "evfilter_struct.h"

struct commit {
	void (*commit)(struct input_event*, void *data);
	void *data;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct commit *tmp = (struct commit*) self->data;

	tmp->commit(ev, tmp->data);

	/* If we are in the middle, send events to next filter */
	if (self->next != NULL)
		self->next->modify(self->next, ev);
}

/*
 * Return void *data, back to user.
 */
static void *filter_free(struct evf_filter *self)
{
	struct commit *tmp = (struct commit*) self->data;

	return tmp->data;
}

struct evf_filter *evf_commit_alloc(void (*commit)(struct input_event*, void *data), void *data)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct commit));
	struct commit *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct commit*) evf->data;

	tmp->commit = commit;
	tmp->data   = data;
	
	evf->modify = modify;
	evf->free   = filter_free;
	evf->name   = "Commit";
	evf->desc   = "Commits events that came trough filters.";
	evf->next   = NULL;

	return evf;
}
