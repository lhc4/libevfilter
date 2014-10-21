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
 * Evfilter GetFromHandle:
 *
 * Parameters:
 *
 * HandleName string
 *
 * Note that the order of the input events may get messed up. Use barrier
 * filters to avoid this.
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_handle.h"
#include "evf_msg.h"

struct priv {
	char name[EVF_HANDLE_MAX];
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	self->next->modify(self->next, ev);
}

static void *filter_free(struct evf_filter *self)
{
	struct priv *priv = (struct priv*) self->data;

	evf_handle_detach(priv->name);

	return NULL;
}

struct evf_filter *evf_get_from_handle_alloc(const char *name)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter)
	                                + sizeof (struct priv));
	struct evf_filter *ret = evf;
	struct priv *priv;

	if (evf == NULL)
		return NULL;

	//TODO: error propagation.
	if (evf_handle_attach(name, &ret, modify)) {
		free(evf);
		return NULL;
	}

	evf->modify = modify;
	evf->free   = filter_free;
	evf->name   = "GetFromHandle";
	evf->desc   = "Gets events from handle.";

	priv = (struct priv*) evf->data;

	strncpy(priv->name, name, EVF_HANDLE_MAX);
	priv->name[EVF_HANDLE_MAX - 1] = '\0';

	return evf;
}

static struct evf_param filter_params[] = {
	{"HandleName", evf_str, NULL, "Name of the handle", NULL },
	{NULL        ,       0, NULL, NULL,                 NULL }
};

struct evf_filter *evf_get_from_handle_creat(char *params, union evf_err *err)
{
	char *name;
	struct evf_filter *evf;

	if (evf_load_params(err, params, filter_params, &name) == -1)
		return NULL;
	
	evf = evf_get_from_handle_alloc(name);
	
	if (evf == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
