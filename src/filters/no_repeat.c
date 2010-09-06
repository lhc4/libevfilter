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
 *
 * Evfilter no_repeat:
 *
 * Ignores repeat events on all keys.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"

static void modify(struct evf_filter *self, struct input_event *ev)
{
	//TODO: ev->value constant here?
	if (ev->type != EV_KEY || ev->value != 2)
		self->next->modify(self->next, ev);
}

struct evf_filter *evf_no_repeat_alloc(void)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter));

	if (evf == NULL)
		return NULL;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "No Repeat";
	evf->desc   = "Throws away all key repeat events.";

	return evf;
}

/* yes, we have no parameters */
static struct evf_param no_repeat_params[] = {
	{ NULL, 0, NULL },
};

struct evf_filter *evf_no_repeat_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;

	if (evf_load_params(err, params, no_repeat_params))
		return NULL;

	evf = evf_no_repeat_alloc();

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
