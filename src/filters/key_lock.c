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
 * Evfilter KeyLock:
 *
 * Parameters:
 * 
 * Key Key
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_handle.h"

struct priv {
	int key;
	int state;
	int eat_next_sync;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct priv *priv = (struct priv*) self->data; 

	if (priv->eat_next_sync) {
		priv->eat_next_sync = 0;

		if (ev->type == 0 && ev->code == 0 && ev->value == 0)
			return;
	}

	if (ev->type == EV_KEY && ev->code == priv->key) {
		/* key released */
		if (ev->value == 0) {
			priv->eat_next_sync = 1;
			return;
		}
		
		/* key pressed */
		if (ev->value == 1) {
			if (priv->state == 0)
				priv->state = 1;
			else {
				priv->state = 0;
				ev->value = 0;
			}
		}
	}
	
	self->next->modify(self->next, ev);
}

struct evf_filter *evf_key_lock_alloc(int key)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter)
	                                + sizeof (struct priv));
	struct priv *priv;

	if (evf == NULL)
		return NULL;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "KeyLock";
	evf->desc   = "Locks key when pressed, unlock when pressed for the "
	              "second time.";

	priv = (struct priv*) evf->data;

	priv->key           = key;
	priv->state         = 0;
	priv->eat_next_sync = 0;

	return evf;
}

static struct evf_param filter_params[] = {
	{"Key", evf_key, NULL, "Key to apply", NULL },
	{NULL ,       0, NULL, NULL, NULL }
};

struct evf_filter *evf_key_lock_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;
	int key;

	if (evf_load_params(err, params, filter_params, &key) == -1)
		return NULL;
	
	evf = evf_key_lock_alloc(key);
	
	if (evf == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
