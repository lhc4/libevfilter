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
 * This filter maps keys on keys
 *
 * filter: KeySubstitute
 *
 * parameters;
 * key1
 * key2
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_handle.h"

struct priv {
	int key_from;
	int key_to;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct priv *priv = (struct priv*) self->data;

	if (ev->type == EV_KEY && ev->code == priv->key_from)
		ev->code = priv->key_to;

	self->next->modify(self->next, ev);
}

struct evf_filter *evf_key_substitute_alloc(int key_from, int key_to)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) +
	                                sizeof (struct priv));
	struct priv *priv;

	if (evf == NULL)
		return NULL;

	priv = (struct priv*) evf->data;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "KeySubstitute";
	evf->desc   = "Substitute one key code for another";

	priv = (struct priv*) evf->data;

	priv->key_from = key_from;
	priv->key_to   = key_to;

	return evf;
}

static struct evf_param filter_params[] = {
	{"KeyFrom", evf_key, NULL},
	{"KeyTo"  , evf_key, NULL},
	{NULL     ,       0, NULL}
};

struct evf_filter *evf_key_substitute_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;
	int key_from, key_to;

        if (evf_load_params(err, params, filter_params, &key_from, &key_to) == -1)
                return NULL;

        evf = evf_key_substitute_alloc(key_from, key_to);

	if (evf == NULL) {
                err->type = evf_errno;
                err->err_no.err_no = errno;
        }

        return evf;
}
