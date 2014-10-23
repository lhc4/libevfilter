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
 * Evfilter weighted_average_abs:
 *
 * Calculates weighted average of absolute X, Y and pressure.
 *
 * samples = number of samples in history
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evf_struct.h"
#include "evf_priv.h"
#include "evf_msg.h"

#define sgn(x) ((x)>0?1:(-1))

struct average {
	int n;
	int head[3];
	int tail[3];
	int coord[][3];
};

static int calculate(struct average *av, int c)
{
	float result = 0;
	int n = 1;
	int i;

	for (i = av->head[c]; i != av->tail[c]; ++i, i %= av->n)
		result = 1.00 / n++ * av->coord[i][c];
	
	return (int) result;
}

static void add(struct average *av, int c, int val)
{
	/* throw away sample on the tail */
	if (abs(av->head[c] - av->tail[c]) >= av->n - 1) {
		++av->tail[c];
		av->tail[c] %= av->n;
	}

	/* move head */
	++av->head[c];
	av->head[c] %= av->n;
	
	/* save sample */
	av->coord[av->head[c]][c] = val; 
}

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct average *average = (struct average*) self->data;

	if (ev->type == EV_ABS)
		switch (ev->code) {
			case ABS_X:
				add(average, 0, ev->value);
				ev->value = calculate(average, 0);
			break;
			case ABS_Y:
				add(average, 1, ev->value);
				ev->value = calculate(average, 1);
			break;
			case ABS_PRESSURE:
				/* pen up */
				if (ev->value == 0) {
					average->head[0] = 0;
					average->tail[0] = 0;
					average->head[1] = 0;
					average->tail[1] = 0;
					average->head[2] = 0;
					average->tail[2] = 0;
				} else {
					add(average, 2, ev->value);
					ev->value = calculate(average, 2);
				}
			break;
	}
	
	self->next->modify(self->next, ev);
}

static void status(struct evf_filter *self, char *buf, int len)
{
	struct average *average = (struct average*) self->data;
	snprintf(buf, len, "weighted_average_abs of last %i values", average->n);
}

struct evf_filter *evf_weighted_average_abs_alloc(unsigned int n)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct average) + sizeof (int) * 3 * n);
	struct average *tmp;

	if (evf == NULL)
		return NULL;

	tmp = (struct average*) evf->data;

	/* number of samples to store */
	tmp->n = n;
	
	/* heads and tails for queunes */
	tmp->head[0] = 0;
	tmp->tail[0] = 0;
	tmp->head[1] = 0;
	tmp->head[1] = 0;
	tmp->tail[2] = 0;
	tmp->tail[2] = 0;

	evf->modify = modify;
	evf->free   = NULL;
	evf->status = status;
	evf->name   = "Weighted Average Abs";
	evf->desc   = "Does exponential average of last n samples.";

	return evf;
}

static struct evf_lim_int samples_lim = { 0, 10 };

static struct evf_param average_params[] = {
	{ "samples", evf_int, &samples_lim, "Number of samples", NULL },
	{ NULL,      0,       NULL,          NULL,               NULL },
};


struct evf_filter *evf_weighted_average_abs_creat(char *params, union evf_err *err)
{
	int nr_samples;
	struct evf_filter *evf;

	if (evf_load_params(err, params, average_params, &nr_samples) == -1)
		return NULL;
	
	evf = evf_weighted_average_abs_alloc((unsigned int)nr_samples);

	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
