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
 * Evfilter rotate:
 *
 *  Exchange absolute or relative possitions events.
 *
 * parameters:
 *
 * rotate_abs_coords = bool
 * rotate_rel_coords = bool
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <limits.h>

#include "evfilter_struct.h"
#include "evfilter_priv.h"

struct rotate {
	int abs_coords;
	int rel_coords;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct rotate *rotate = (struct rotate*) self->data;

	/*
	 * Exchange relative events.
	 */
	if (ev->type == EV_REL) {
		if (rotate->rel_coords)
			switch (ev->code) {
				case REL_X:
					ev->code = REL_Y;
				break;
				case REL_Y:
					ev->code = REL_X;
				break;
			}
	}

	/*
	 * Exchange absolute events.
	 */
	if (ev->type == EV_ABS) {
		if (rotate->abs_coords)
			switch (ev->code) {
				case ABS_X:
					ev->code = ABS_Y;
				break;
				case ABS_Y:
					ev->code = ABS_X;
				break;
			}
	}
	
	self->next->modify(self->next, ev);
}

struct evf_filter *evf_rotate_alloc(int rotate_abs_coords, int rotate_rel_coords)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct rotate));
	struct rotate *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct rotate*) evf->data;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Rotate";
	evf->desc   = "Exchanges relative and/or absoulte possitions.";

	tmp->abs_coords = rotate_abs_coords;
	tmp->rel_coords = rotate_rel_coords;

	return evf;
}

static struct evf_param rotate_params[] = {
	{ "RotateAbs", evf_bool, NULL },
	{ "RotateRel", evf_bool, NULL },
	{ NULL       ,        0, NULL },
};

struct evf_filter *evf_rotate_creat(char *params, union evf_err *err)
{
	int rotate_abs_coords, rotate_rel_coords;
	struct evf_filter *evf;

	if (evf_load_params(err, params, rotate_params, &rotate_abs_coords, &rotate_rel_coords) == -1)
		return NULL;
	
	evf = evf_rotate_alloc(rotate_abs_coords, rotate_rel_coords);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
