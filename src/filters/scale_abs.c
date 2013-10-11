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

#include "evf_struct.h"
#include "evf_priv.h"

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
				ev->value = 1.00 * INT_MAX * (ev->value - scale->minx) / scale->maxx;
			break;
			case ABS_Y:
				ev->value = 1.00 * INT_MAX * (ev->value - scale->miny) / scale->maxy;
			break;
			case ABS_PRESSURE:
//				if (ev->value != 0)
//					ev->value = (ev->value - scale->minp) * (INT_MAX / scale->maxp);
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
	{ "maxx", evf_int, NULL, "Maximal X from device",        NULL    },
	{ "maxy", evf_int, NULL, "Maximal Y from device",        NULL    },
	{ "maxp", evf_int, NULL, "Maximal pressure from device", "32767" },
	{ "minx", evf_int, NULL, "Minimal X from device",        "0"     },
	{ "miny", evf_int, NULL, "Minimal Y from device",        "0"     },
	{ "minp", evf_int, NULL, "Minimal pressure from device", "32767" },
	{ NULL,   0,       NULL, NULL,                           NULL    },
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
