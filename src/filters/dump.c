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
 * Copyright (C) 2008 Cyril Hrubis                                            *
 *                                                                            *
 ******************************************************************************/

/*
 * Evfilter dump:
 *
 * Writes nice decomposition of event to the FILE.
 * 
 * parameters:
 *
 * prefix = string 
 *  prefix that is printed in every print
 *
 * file = path
 *  path to file to print to, there are two special files stdout and stderr
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>

#include "evfilter_struct.h"
#include "evfilter_priv.h"
#include "linux_input.h"

struct dump {
	FILE *f;
	char prefix[];
};

static void modify(struct evf_filter *self, struct input_event *ev)
{
	struct dump *dump = (struct dump*) self->data;

	evf_input_print(dump->f, dump->prefix, ev);
	fprintf(dump->f, "\n");

	if (self->next != NULL)
		self->next->modify(self->next, ev);
}

struct evf_filter *evf_dump_alloc(char *prefix, FILE *f)
{
	struct evf_filter *evf = malloc(sizeof (struct evf_filter) + sizeof (struct dump) + strlen(prefix) + 1);
	struct dump *tmp;

	if (evf == NULL)
		return NULL;
	
	tmp = (struct dump*) evf->data;
	
	strcpy(tmp->prefix, prefix);

	tmp->f      = f;
	evf->modify = modify;
	evf->free   = NULL;
	evf->name   = "Dump";
	evf->desc   = "Prints text variant of event into file.";

	return evf;
}

static struct evf_param dump_params[] = {
	{ "prefix", evf_str , NULL },
	{ "file"  , evf_file, NULL },
	{ NULL    ,        0, NULL },
};

struct evf_filter *evf_dump_creat(char *params, union  evf_err *err)
{
	char *prefix;
	FILE *file;
	struct evf_filter *evf;

	if (evf_load_params(err, params, dump_params, &prefix, &file))
		return NULL;
	
	evf = evf_dump_alloc(prefix, file);
	
	if (evf == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
	}

	return evf;
}
