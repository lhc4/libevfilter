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
 * Evfilter: pressure_to_key
 *
 * Generates button events from pressure.
 *
 * parameters:
 *
 * treshold = Integer
 *  treshold value, when pressure exceeds this value BTN_DOWN is generated
 *  If pressure is goes under treshold BTN_UP is generated.
 *
 * key = key
 *  Name of the key, to be generated.
 */

#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_msg.h"

struct pressure {
	int treshold;
	int key;

	int pressed;
};

static int check_treshold(int value, int treshold)
{
	if (treshold < 0) {
		if (value == 0)
			return 0;
		return value < abs(treshold);
	}

	return value > treshold;
}

/*
 * Generate key down, up when treshold was reached.
 * TODO: should we eat pressure events?
 *       generate sync event!
 */
static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct pressure *p = (struct pressure*) self->data;

	/* send pressure event */
	self->next->modify(self->next, ev);

	/* generate key event */
	if (ev->type == EV_ABS && ev->code == ABS_PRESSURE) {
		if (check_treshold(ev->value, p->treshold)) {
			if (!p->pressed) {

				p->pressed = 1;
				ev->type   = EV_KEY;
				ev->code   = p->key;
				ev->value  = 1;

				self->next->modify(self->next, ev);
			}
		} else {
			if (p->pressed) {
				
				p->pressed = 0;
				ev->type   = EV_KEY;
				ev->code   = p->key;
				ev->value  = 0;
				
				self->next->modify(self->next, ev);
			}
		}
	}
}

struct evf_filter *evf_pressure_to_key_alloc(int treshold, int key)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct pressure));
	struct pressure *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct pressure*) evf->data;
	
	tmp->treshold = treshold;
	tmp->key      = key;
	tmp->pressed  = 0;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Pressure To Key";
	evf->desc   = "Converts touchscreen pressure to key events.";

	return evf;
}

static struct evf_param pressure_to_key_params[] = {
	{ "treshold", evf_int, NULL, "Threshold for activating", NULL },
	{ "key"     , evf_key, NULL, "Key to be sent",           NULL },
	{ NULL      ,       0, NULL, NULL,                       NULL },
};

struct evf_filter *evf_pressure_to_key_creat(char *params, union evf_err *err)
{
	int treshold;
	int key;
	struct evf_filter *evf;

	if (evf_load_params(err, params, pressure_to_key_params, &treshold, &key) == -1)
		return NULL;

	evf = evf_pressure_to_key_alloc(treshold, key);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
