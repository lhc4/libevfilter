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

#include <stdlib.h>
#include <string.h>

#include "evf_struct.h"
#include "evf_handle.h"

static struct evf_handle *handles = NULL;

static struct evf_handle *handle_alloc(const char *name)
{
	struct evf_handle *new = malloc(sizeof (struct evf_handle));

	if (new == NULL)
		return NULL;

	strncpy(new->name, name, EVF_HANDLE_MAX);

	new->name[EVF_HANDLE_MAX - 1] = '\0';
	new->filter                   = NULL;
	new->send                     = NULL;
	new->refcount                 = 0;

	new->next = handles;
	handles   = new;

	return new;
}

void handle_free(struct evf_handle *self)
{
	struct evf_handle *i;
	
	/* can't happen */
	if (handles == NULL)
		return;

	for (i = handles; i != NULL; i++)
		if (i->next == self)
			break;

	/* can't happen either */
	if (i == NULL)
		return;

	if (i == handles)
		handles = self->next;
	else
		i->next = self->next;
	
	free(self);
}

static struct evf_handle *handle_lookup(const char *name)
{
	struct evf_handle *i;

	for (i = handles; i != NULL; i = i->next)
		if (!strcasecmp(i->name, name))
			return i;

	return NULL;
}

struct evf_handle *evf_handle_create(const char *name)
{
	struct evf_handle *i = handle_lookup(name);

	if (i != NULL) {
		i->refcount++;
		return i;
	}

	i = handle_alloc(name);

	if (i == NULL)
		return NULL;

	i->refcount++;

	return i;
}

void evf_handle_destroy(struct evf_handle *self)
{
	if (self->refcount > 1) {
		self->refcount--;
		return;
	}

	if (self->filter == NULL)
		handle_free(self);
}

void evf_handle_detach(const char *name)
{
	struct evf_handle *i = handle_lookup(name);

	if (i == NULL)
		return;

	if (i->refcount > 0) {
		i->filter = NULL;
		return;
	}

	handle_free(i);
}

int evf_handle_attach(const char *name, struct evf_filter **filter,
                      void (*send)(struct evf_filter *self,
                                   struct input_event *ev))
{
	struct evf_handle *i = handle_lookup(name);

	if (i == NULL) {
		i = handle_alloc(name);
		
		if (i == NULL)
			return 1;

		i->filter = *filter;
		i->send   = send;
		return 0;
	}

	if (i->filter == NULL) {
		i->filter = *filter;
		i->send   = send;
		return 0;
	}

	*filter = i->filter;
	return 2;
}

void evf_handle_send(struct evf_handle *self, struct input_event *ev)
{
	if (self->filter == NULL)
		return;

	self->send(self->filter, ev);
}
