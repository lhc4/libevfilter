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
 * Copyright (C) 2008-2009 Cyril Hrubis                                       *
 *                                                                            *
 ******************************************************************************/

/*
 * Evfilter btn2rel:
 *
 * Converts keys to relative movement.
 *
 * parameters:
 *
 * KeyUp    = Key
 *  Key to cause movement up.
 *
 * KeyDown  = Key
 *  Key to cause movement down.
 * 
 * KeyLeft  = Key
 *  Key to cause movement left.
 *
 * KeyRight = Key
 *  Key to cause movements right.
 *
 * LeftBtn   = Key
 *  Key to be translated to mouse left button.
 *
 * LightBtn  = Key
 *  Key to be translated to mouse right button.
 *
 * MiddleBtn = Key
 *  Key to be translated to mouse middle button.
 *
 */

#include <stdlib.h>
#include <linux/input.h>
#include <errno.h>

#include "evfilter_struct.h"
#include "evfilter_priv.h"

struct btn2rel {
	/* movement keys */
	int key_up;
	int key_down;
	int key_left;
	int key_right;
	/* mouse button keys */
	int right_btn;
	int middle_btn;
	int left_btn;

	int eat_next_syn;
};

/*
 * We replace key down events by relative movement events here. We are ignoring
 * key up events on keys defined to be used to generate events. Still we are
 * sending one more sync after key up event. TODO: can this cause problems?
 */
static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct btn2rel *data = (struct btn2rel*) self->data;
	
	/* eat syn event after key_up */
	if (ev->type == EV_SYN && data->eat_next_syn) {
		data->eat_next_syn = 0;
		return;
	}

	if (ev->type == EV_KEY) {
	
		if (ev->code == data->key_up) {
			
			/* ignore key up events */
			if (ev->value == 0) {
				data->eat_next_syn = 1;
				return;
			}
			
			ev->type  = EV_REL;
			ev->code  = REL_Y;
			ev->value = 2;
		}

		if (ev->code == data->key_down) {
			
			/* ignore key up events */
			if (ev->value == 0) {
				data->eat_next_syn = 1;
				return;
			}

			ev->type  = EV_REL;
			ev->code  = REL_Y;
			ev->value = -2;
		}

		if (ev->code == data->key_left) {

			/* ignore key up events */
			if (ev->value == 0) {
				data->eat_next_syn = 1;
				return;
			}

			ev->type  = EV_REL;
			ev->code  = REL_X;
			ev->value = -2;
		}

		if (ev->code == data->key_right) {
			
			/* ignore key up events */
			if (ev->value == 0) {
				data->eat_next_syn = 1;
				return;
			}

			ev->type  = EV_REL;
			ev->code  = REL_X;
			ev->value = 2;
		}

		if (ev->code == data->left_btn)
			ev->code = BTN_LEFT;

		if (ev->code == data->middle_btn)
			ev->code = BTN_MIDDLE;
		
		if (ev->code == data->right_btn)
			ev->code = BTN_RIGHT;

	}

	self->next->modify(self->next, ev);
}

struct evf_filter *evf_btn2rel_alloc(int key_down, int key_up, int key_left, int key_right, int left_btn, int middle_btn, int right_btn)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct btn2rel));
	struct btn2rel *data;

	if (evf == NULL)
		return NULL;

	data = (struct btn2rel*) evf->data;

	data->key_up       = key_up;
	data->key_down     = key_down;
	data->key_left     = key_left;
	data->key_right    = key_right;
	data->left_btn     = left_btn;
	data->middle_btn   = middle_btn;
	data->right_btn    = right_btn;
	data->eat_next_syn = 0;

	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Btn2Rel";
	evf->desc   = "Translates pressed keys to relative movements.";

	return evf;
}

static struct evf_param btn2rel_params[] = {
	{ "keyup"    , evf_key, NULL },
	{ "keydown"  , evf_key, NULL },
	{ "keyleft"  , evf_key, NULL },
	{ "keyright" , evf_key, NULL },
	{ "leftbtn"  , evf_key, NULL },
	{ "middlebtn", evf_key, NULL },
	{ "rightbtn" , evf_key, NULL },
	{ NULL       ,       0, NULL },
};

struct evf_filter *evf_btn2rel_creat(char *params, union evf_err *err)
{
	int key_up, key_down, key_left, key_right, left_btn, middle_btn, right_btn;
	struct evf_filter *evf;

	if (evf_load_params(err, params, btn2rel_params, &key_up, &key_down, &key_left, &key_right, &left_btn, &middle_btn, &right_btn) == -1)
		return NULL;
	
	evf = evf_btn2rel_alloc(key_up, key_down, key_left, key_right, left_btn, middle_btn, right_btn);
	
	if (evf == NULL) {
		err->type          = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
