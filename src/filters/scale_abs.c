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
 * Evfilter scale_abs:
 *
 * scales abs events and normalize all values betwen 0 and MAX_INT
 *
 * parameters:
 *  maxx = maximal x value
 *  maxy = maximal y value
 *  maxp = maximal pressure
 *  minx = minimal x value
 *  miny = minimal y value
 *  minp = minimal pressure
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <limits.h>

#include "evfilter_struct.h"
#include "evfilter_priv.h"

struct scale {
	int maxx;
	int maxy;
	int maxp;
	int minx;
	int miny;
	int minp;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct scale *scale = (struct scale*) self->data;

	if (ev->type == EV_ABS)
		switch (ev->code) {
			case ABS_X:
				ev->value = (ev->value - scale->minx) * (INT_MAX / scale->maxx);
			break;
			case ABS_Y:
				ev->value = (ev->value - scale->miny) * (INT_MAX / scale->maxy);
			break;
			case ABS_PRESSURE:
				if (ev->value != 0)
					ev->value = (ev->value - scale->minp) * (INT_MAX / scale->maxp);
			break;
	}
	
	self->next->modify(self->next, ev);
}

struct evf_filter *evf_scale_abs_alloc(int maxx, int maxy, int maxp, int minx, int miny, int minp)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct scale));
	struct scale *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct scale*) evf->data;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Scale";
	evf->desc   = "Linear touchscreen cablibration.";

	tmp->maxx = maxx;
	tmp->maxy = maxy;
	tmp->maxp = maxp;
	tmp->minx = minx;
	tmp->miny = miny;
	tmp->minp = minp;

	return evf;
}

static struct evf_param scale_params[] = {
	{ "maxx", evf_int, NULL },
	{ "maxy", evf_int, NULL },
	{ "maxp", evf_int, NULL },
	{ "minx", evf_int, NULL },
	{ "miny", evf_int, NULL },
	{ "minp", evf_int, NULL },
	{ NULL,   0,       NULL },
};

struct evf_filter *evf_scale_abs_creat(char *params, union evf_err *err)
{
	int maxx, maxy, maxp;
	int minx, miny, minp;
	struct evf_filter *evf;

	if (evf_load_params(err, params, scale_params, &maxx, &maxy, &maxp, &minx, &miny, &minp) == -1)
		return NULL;
	
	evf = evf_scale_abs_alloc(maxx, maxy, maxp, minx, miny, minp);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
