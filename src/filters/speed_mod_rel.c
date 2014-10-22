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
 * Evfilter speed_mod_rel:
 *
 * lower speed on relative pointer
 *
 * parameters:
 *
 * xmod = Integer
 *  modifier on x
 *
 * ymod = Integer
 *  modifier on y
 *
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_msg.h"

#define sgn(x) ((x)>0?1:(-1))

struct relspeed {
	int x;
	int y;
	int xmod;
	int ymod;

	int eat_next_syn;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct relspeed *relspeed = (struct relspeed*) self->data;

	/* eat right syn events */
	if (ev->type == EV_SYN && relspeed->eat_next_syn) {
		relspeed->eat_next_syn = 0;
		return;
	}

	if (ev->type == EV_REL)
		switch (ev->code) {
			case REL_X:
				relspeed->x += ev->value;
				if (abs(relspeed->x) >= relspeed->xmod) {
					ev->value = relspeed->x / relspeed->xmod;
					relspeed->x = sgn(relspeed->x)*(abs(relspeed->x) % relspeed->xmod);
				} else {
					relspeed->eat_next_syn = 1;
					return;
				}
			break;
			case REL_Y:
				relspeed->y += ev->value;
				if (abs(relspeed->y) >= relspeed->ymod) {
					ev->value = relspeed->y / relspeed->ymod;
					relspeed->y = sgn(relspeed->y)*(abs(relspeed->y) % relspeed->ymod);
				} else {
					relspeed->eat_next_syn = 1;
					return;
				}
			break;

	}

	self->next->modify(self->next, ev);
}

struct evf_filter *evf_speed_mod_rel_alloc(int xmod, int ymod)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct relspeed));
	struct relspeed *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct relspeed*) evf->data;
	
	tmp->x     = 0;
	tmp->y     = 0;
	tmp->xmod  = xmod;
	tmp->ymod  = ymod;
	
	tmp->eat_next_syn = 0;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "SpeedMod";
	evf->desc   = "Slows down your mouse.";

	return evf;
}

static struct evf_param relspeed_params[] = {
	{ "xmod", evf_int, NULL, "X movement divisor", "1" },
	{ "ymod", evf_int, NULL, "Y movement divisor", "1" },
	{ NULL  ,       0, NULL, NULL, NULL },
};

struct evf_filter *evf_speed_mod_rel_creat(char *params, union evf_err *err)
{
	int xmod;
	int ymod;
	struct evf_filter *evf;

	if (evf_load_params(err, params, relspeed_params, &xmod, &ymod) == -1)
		return NULL;
	
	evf = evf_speed_mod_rel_alloc(xmod, ymod);
	
	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
