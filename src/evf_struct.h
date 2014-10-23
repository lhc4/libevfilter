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
  
  Here comes struct evfilter definition, do not inlucde this file into any
  code outside of this library. To acces it's mebers use functions defined
  in evf_filter.h.
  
 */

#ifndef __EVF_STRUCT_H__
#define __EVF_STRUCT_H__

struct input_event;

/*
 * Main evfilter filter structure.
 */
struct evf_filter {
	/*
	 * This function is invoked from previous filter and your filter
	 * should invoke self->next->modify(self->next, ev) on any event
	 * that was generated/passed trough your filter.
	 */
	void (*modify)(struct evf_filter *self, struct input_event *ev);
	/* Either NULL, or additional free helper */
	void *(*free)(struct evf_filter *self);
	/* Either NULL, or function to print status */
	void (*status)(struct evf_filter *self, char *buf, int len);
	/* Filter name */
	char *name;
	/* Filter description */
	char *desc;
	/* Next filter in filter line. */
	struct evf_filter *next;
	/* 
	 * Iternal filter data, usually structure holding filter state. 
	 * In case filter is not stateless.
	 */
	char data[0];
};

#endif /* __EVF_STRUCT_H__ */
