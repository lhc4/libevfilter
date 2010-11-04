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

#ifndef __EVF_PARAM_H__
#define __EVF_PARAM_H__

/*
 * Parameters filter configuration loaders
 */
enum evf_param_t {
	evf_key = 0, /* see keys.h             */
	evf_evtype,  /* event type             */
	evf_keys,    /* field of keys          */
	evf_int,     /* just number            */
	evf_float,   /* float                  */
	evf_str,     /* null terminated string */
	evf_file,    /* struct FILE*           */
	evf_bool,    /* bool stored in int     */
};

/*
 * Structures used to define filter parameters.
 */
struct evf_param {
	char *name;
	enum evf_param_t type;
	void *lim;
};

/*
 * Limits
 */
struct evf_lim_int {
	int min;
	int max;
};

struct evf_lim_str {
	int min_len;
	int max_len;
};

/*
 * Returns name for type.
 */
const char *evf_get_type_name(enum evf_param_t type);

#endif /* __EVF_PARAM_H__ */
