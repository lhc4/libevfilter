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
 *
 * Evfilter FilterToHandle:
 *
 * Send one type of events to handle.
 *
 * Parameters:
 * EventType  evtype
 * EventCode  int
 * HandleName string
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <limits.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_handle.h"
#include "evf_msg.h"

extern char **evtypes;

struct priv {
	int type;
	int code;
	int sync_flag;
	int ev_flag;
	struct evf_handle *handle;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct priv *priv = (struct priv*) self->data;

	if (ev->type == 0 && ev->code == 0 && ev->value == 0
	    && priv->sync_flag) {

		evf_handle_send(priv->handle, ev);

		if (priv->ev_flag)
			self->next->modify(self->next, ev);

		return;
	}

	if (ev->type == priv->type && ev->code == priv->code) {
		evf_handle_send(priv->handle, ev);
		priv->sync_flag = 1;
		priv->ev_flag = 0;
	} else {
		self->next->modify(self->next, ev);
		priv->ev_flag = 1;
	}
}

static void *filter_free(struct evf_filter *self)
{
	struct priv *priv = (struct priv*) self->data;

	evf_handle_destroy(priv->handle);

	return NULL;
}

static void status(struct evf_filter *self, char *buf, int len)
{
	struct priv *priv = (struct priv*) self->data;
	snprintf(buf, len, "Sending events of type %s code %i to handle '%s'",
		evtypes[priv->type], priv->code, (priv->handle ? priv->handle->name : "NULL"));
}

struct evf_filter *evf_filter_to_handle_alloc(const char *name, int type, int code)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter)
	                                + sizeof (struct priv));
	struct priv *priv;

	if (evf == NULL)
		return NULL;

	priv = (struct priv*) evf->data;

	priv->handle = evf_handle_create(name);

	if (priv->handle == NULL) {
		free(evf);
		return NULL;
	}

	evf->modify = modify;
	evf->free   = filter_free;
	evf->status = status;
	evf->name   = "FilterToHandle";
	evf->desc   = "Redirect some type of events to handle.";

	priv->type = type;
	priv->code = code;

	priv->sync_flag = 0;

	return evf;
}

static struct evf_param filter_params[] = {
	{"HandleName", evf_str   , NULL, "Name of the handle",        NULL },
	{"EventType" , evf_evtype, NULL, "Type of redirected events", NULL },
	{"EventCode" , evf_int   , NULL, "Code of redirected events", NULL },
	{        NULL,       0   , NULL, NULL,                        NULL },
};

struct evf_filter *evf_filter_to_handle_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;
	int type, code;
	char *name;

	if (evf_load_params(err, params, filter_params, &name, &type, &code) == -1)
		return NULL;

	evf = evf_filter_to_handle_alloc(name, type, code);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
