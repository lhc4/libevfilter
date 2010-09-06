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
 * Evfilter kalman:
 *
 * Uses kalman filter with gain kx and ky.
 *
 * kx -- gain for x
 * ky -- gain for y
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"

struct kalman {
	float kx;
	float ky;

	float x, vx;
	float y, vy;

	int reset_x;
	int reset_y;
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct kalman *kalman = (struct kalman*) self->data;

	if (ev->type == EV_ABS) {
		switch (ev->code) {
			case ABS_X:
				if (kalman->reset_x) {
					kalman->x = ev->value;
					kalman->reset_x = 0;
				} else {
					float est_x = kalman->x + kalman->vx;
					float new_x = est_x + kalman->kx * (ev->value - est_x);
					kalman->vx  = kalman->vx/10 - 0.1 * (est_x - ev->value);
					kalman->x = new_x;
					ev->value = (int) new_x;
				}
			break;
			case ABS_Y:
				if (kalman->reset_y) {
					kalman->y = ev->value;
					kalman->reset_y = 0;
				} else {
					float est_y = kalman->y + kalman->vy;
					float new_y = est_y + kalman->ky * (ev->value - est_y);
					kalman->vy = kalman->vy/10 - 0.1 * (est_y - ev->value);
					kalman->y = new_y;
					ev->value = (int) new_y;
				}
			break;
			case ABS_PRESSURE:
				/* pen up */
				if (ev->value == 0) {
					kalman->reset_x = 1;
					kalman->reset_y = 1;
					kalman->vx = 0;
					kalman->vy = 0;
				}
			break;
		}
	}

	self->next->modify(self->next, ev);
}

struct evf_filter *evf_kalman_alloc(float kx, float ky)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct kalman));
	struct kalman *kalman;

	if (evf == NULL)
		return NULL;

	kalman = (struct kalman*) evf->data;

	kalman->kx = kx;
	kalman->ky = ky;
	kalman->vx = 0;
	kalman->vy = 0;
	kalman->reset_x = 1;
	kalman->reset_y = 1;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Kalman";
	evf->desc   = "Kalman filter.";

	return evf;
}

static struct evf_param kalman_params[] = {
	{ "kx", evf_float, NULL },
	{ "ky", evf_float, NULL },
	{ NULL,         0, NULL },
	{ NULL,         0, NULL },
};


struct evf_filter *evf_kalman_creat(char *params, union evf_err *err)
{
	struct evf_filter *evf;
	float kx, ky; 

	if (evf_load_params(err, params, kalman_params, &kx, &ky) == -1)
		return NULL;

	evf = evf_kalman_alloc(kx, ky);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
