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
 * Evfilter rel2scroll:
 *
 * parameters:
 *
 * TriggerButton -- button that switch between scrolling and pointer movements.
 * 
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>
#include <stdbool.h>

#include "evf_struct.h"
#include "evf_priv.h"

struct rel2scroll {
	int x;
	int y;
	int  trigger_btn;
	bool trigger_on;
	bool eat_next_sync;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct rel2scroll *data = (void*) self->data;

	if (ev->type == EV_SYN && data->eat_next_sync) {
		data->eat_next_sync = false;
		return;
	}

	data->eat_next_sync = false;

	if (ev->type == EV_KEY && ev->code == data->trigger_btn) {
		
		if (ev->value == 0)
			data->trigger_on = false;
		else
			data->trigger_on = true;

		data->eat_next_sync = true;
		return;
	}

	if (data->trigger_on) {
		if (ev->type == EV_REL) {
			switch (ev->code) {
				case REL_X:
					ev->code = REL_HWHEEL;
				break;
				case REL_Y:
					ev->code = REL_WHEEL;
				break;
			}
		}
	}

	self->next->modify(self->next, ev);
}

struct evf_filter *evf_rel2scroll_alloc(int trigger_btn)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) +
	                                sizeof (struct rel2scroll));
	struct rel2scroll *data;

	if (evf == NULL)
		return NULL;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Rel2Scroll";
	evf->desc   = "Converts relative events into scroll events.";

	data = (void*) evf->data;
	
	data->trigger_on    = false;
	data->eat_next_sync = false;
	data->trigger_btn   = trigger_btn;

	return evf;
}

static struct evf_param rel2scroll_params[] = {
	{"TriggerButton", evf_key, NULL},
	{NULL,                  0, NULL},
};

struct evf_filter *evf_rel2scroll_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;
	int trigger_btn;

	if (evf_load_params(err, params, rel2scroll_params, &trigger_btn) == -1)
		return NULL;
	
	evf = evf_rel2scroll_alloc(trigger_btn);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
