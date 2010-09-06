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
 * Copyright (C) 2008 Cyril Hrubis                                            *
 *                                                                            *
 ******************************************************************************/

/*
 * This filter maps keys on keys
 *
 * filter: KeySubstitute
 *
 * parameters;
 * field of keys
 * field of keys
 *
 */

#include <stdlib.h>
#include <linux/input.h>

#include "evf_priv.h"

struct key_substitute {
	int *from;
	int *to;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	int i;
	int *keys = (int*) self->data;

	if (ev->type == EV_KEY)
		for (i = 0; i < keys[0]; i++)
			if (ev->code == keys[2*i+1]) {
				ev->code = keys[2*i+2];
				break;
			}
	
	self->next->modify(self->next, ev);
}

struct evfilter *evf_key_substitute_alloc(int *from, int *to)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct key_substitute));
	struct key_substitute *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct key_substitute*) evf->data;

	tmp[0] = count;
	memcpy(tmp + 1, keys, 2 * count * sizeof (int));

	evf->modify = modify;
	evf->name   = "KeySubstitute";

	return evf;
}
