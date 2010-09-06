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
 * Evfilter mirror:
 *
 *  Mirror absolute or relative possitions events.
 *
 *  NOTE: if you want mirror absolute possitions this filter must be loaded after scale_abs.
 *
 * parameters:
 *
 * mir_abs_x = bool
 * mir_abs_y = bool
 * mir_rel_x = bool
 * mir_rel_y = bool
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <limits.h>

#include "evf_struct.h"
#include "evf_priv.h"

struct mirror {
	int abs_x;
	int abs_y;
	int rel_x;
	int rel_y;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct mirror *mirror = (struct mirror*) self->data;

	/*
	 * Mirror relative events by multiplying by -1
	 */
	if (ev->type == EV_REL) {
		switch (ev->code) {
			case REL_X:
				if (mirror->rel_x)
					ev->value *= -1;
			break;
			case REL_Y:
				if (mirror->rel_y)
					ev->value *= -1;
			break;
		}
	}

	/*
	 * Mirror absolute events. We suppose here that events are normalized,
	 * so values are betwen 0 and INT_MAX.
	 */
	if (ev->type == EV_ABS)
		switch (ev->code) {
			case ABS_X:
				if (mirror->abs_x)
					ev->value = INT_MAX - ev->value;
			break;
			case ABS_Y:
				if (mirror->abs_y)
					ev->value = INT_MAX - ev->value;
			break;
	}
	
	self->next->modify(self->next, ev);
}

struct evf_filter *evf_mirror_alloc(int mir_rel_x, int mir_rel_y, int mir_abs_x, int mir_abs_y)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct mirror));
	struct mirror *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct mirror*) evf->data;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Mirror";
	evf->desc   = "Mirrors absolute or relative events.";

	tmp->abs_x = mir_abs_x;
	tmp->abs_y = mir_abs_y;
	tmp->rel_x = mir_rel_x;
	tmp->rel_y = mir_rel_y;

	return evf;
}

static struct evf_param mirror_params[] = {
	{ "MirrorRelX", evf_bool, NULL },
	{ "MirrorRelY", evf_bool, NULL },
	{ "MirrorAbsX", evf_bool, NULL },
	{ "MirrorAbsY", evf_bool, NULL },
	{         NULL,        0, NULL },
};

struct evf_filter *evf_mirror_creat(char *params, union evf_err *err)
{
	int mir_rel_x, mir_rel_y;
	int mir_abs_x, mir_abs_y;
	struct evf_filter *evf;

	if (evf_load_params(err, params, mirror_params, &mir_rel_x, &mir_rel_y, &mir_abs_x, &mir_abs_y) == -1)
		return NULL;

	evf = evf_mirror_alloc(mir_rel_x, mir_rel_y, mir_abs_x, mir_abs_y);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
