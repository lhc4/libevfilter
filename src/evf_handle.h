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

   Handle is named object that connects output of special fiter to input of any
   filter. 

   Handle is identified by it's name, name is not case sensitive.

   Handle is created either by filter that redirects events to it, or by filter
   asking for events from handle. This way we do not depend on order of handle
   create/attach calls which is especially usefull when we may want to hotplug
   some input devices that exports handles. The only problem here is that typos
   in handle names are harder to debug.

  */

#ifndef __EVF_HANDLE_H__
#define __EVF_HANDLE_H__

#define EVF_HANDLE_MAX 64

struct evf_filter;
struct input_event;

struct evf_handle {
	char name[EVF_HANDLE_MAX];
	int refcount;
	
	/*
	 * End of the handle where events are send.
	 */
	struct evf_filter *filter;
	void (*send)(struct evf_filter *self, struct input_event *ev);

	struct evf_handle *next;
};

/*
 * Create handle, the handle could actually exist with other end waiting on it,
 * in this case only pointer to existing handle is returned. If there is no
 * such handle, handle is allocated.
 *
 * Returns either pointer to handle or in case of malloc(3) failure NULL
 */
struct evf_handle *evf_handle_create(const char *name);

/*
 * Destroy handle. Actually just decreases refcount, the handle is freed when
 * all inputs and output are deregistered from it.
 */
void evf_handle_destroy(struct evf_handle *self);

/*
 * Attach filter to handle. If there is no handle with this name, the handle
 * is allocated so that other end could connect with it.
 *
 * Return values:
 *
 *  0 on succes.
 *  1 on malloc(3) failure.
 *  2 if there is filter allready attached and pointer to it filled into the
 *    filter parameter.
 */
int evf_handle_attach(const char *name, struct evf_filter **filter,
                      void (*send)(struct evf_filter *self,
		                   struct input_event *ev));

/*
 * Removes filter association. If there was no other end, the handle is freed.
 */
void evf_handle_detach(const char *name); 

/*
 * Send event to handle.
 */
void evf_handle_send(struct evf_handle *self, struct input_event *ev);

#endif /* __EVF_HANDLE_H__ */
