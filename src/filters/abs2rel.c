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
 * Evfilter abs2rel:
 *
 * Translates absolute events to relative accordingly to difference of
 * abs coordinates.
 *
 * parameters:
 *
 * none
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"

#define INCIF(x) ((x) < 2 ? ((x)++) : (x));

struct abs2rel {
	int xstate;
	int ystate;
	int relx;
	int rely;
	int last_absx;
	int last_absy;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct abs2rel *data = (struct abs2rel*) self->data;

	/* flush what we have */
	if (ev->type == EV_SYN) {
		
		if (data->xstate == 2) {
			ev->code  = REL_X;
			ev->value = data->relx;
			self->next->modify(self->next, ev);
		}

		if (data->ystate == 2) {
			ev->code  = REL_Y;
			ev->value = data->rely;
			self->next->modify(self->next, ev);
		}
	
		ev->type  = EV_SYN;
		ev->value = 0;
		self->next->modify(self->next, ev);
	}

	if (ev->type == EV_ABS)
		switch (ev->code) {
			case ABS_X:
				data->relx = ev->value - data->last_absx;
				data->last_absx = ev->value;
				INCIF(data->xstate);
			break;
			case ABS_Y:
				data->rely = ev->value - data->last_absy;
				data->last_absy = ev->value;
				INCIF(data->ystate);
			break;
			case ABS_PRESSURE:
				/* pen up */
				if (ev->value == 0) {
					data->xstate = 0;
					data->ystate = 0;
				}
			break;
	} else
		self->next->modify(self->next, ev);
}

struct evf_filter *evf_abs2rel_alloc(void)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct abs2rel));
	struct abs2rel *data;

	if (evf == NULL)
		return NULL;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Abs2Rel";
	evf->desc   = "Converts absolute possition events to relative.";

	data = (struct abs2rel*) evf->data;
	
	data->xstate = 0;
	data->ystate = 0;

	return evf;
}

static struct evf_param abs2rel_params[] = {
	{NULL, 0, NULL, NULL, NULL},
};

struct evf_filter *evf_abs2rel_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;

	if (evf_load_params(err, params, abs2rel_params) == -1)
		return NULL;
	
	evf = evf_abs2rel_alloc();

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
