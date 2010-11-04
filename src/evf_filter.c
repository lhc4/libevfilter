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
#include <stdio.h>
#include <string.h>

#include "evf_filter.h"
#include "filters/filters.h"
#include "evf_err.h"
#include "evf_struct.h"

/*
 * Global tables
 */
static const char *evf_filters[] = 
{ 
 "Dump"              , 
 "NoRepeat"          , 
 "PressureToKey"     , 
 "SpeedMod"          , 
 "Barrier"           , 
 "WeightedAverageAbs", 
 "ScaleAbs"          , 
 "Mirror"            , 
 "Rotate"            , 
 "Abs2Rel"           , 
 "Btn2Rel"           , 
 "Kalman"            , 
 "Rel2Scroll"        , 
 "CopyToHandle"      , 
 "GetFromHandle"     , 
 NULL                ,
};

static struct evf_filter* (*evf_creat[])(char *, union evf_err *) = 
{ 
 evf_dump_creat                , 
 evf_no_repeat_creat           , 
 evf_pressure_to_key_creat     , 
 evf_speed_mod_rel_creat       , 
 evf_barrier_creat             , 
 evf_weighted_average_abs_creat, 
 evf_scale_abs_creat           , 
 evf_mirror_creat              , 
 evf_rotate_creat              ,
 evf_abs2rel_creat             ,
 evf_btn2rel_creat             ,
 evf_kalman_creat              ,
 evf_rel2scroll_creat          ,
 evf_copy_to_handle_creat      ,
 evf_get_from_handle_creat     ,
};

/*
 * Returns filter possition in global table
 */
static int getfilter(const char *name)
{
	int i = 0;

	while (evf_filters[i] != NULL) {
		if (!strcasecmp(name, evf_filters[i]))
			return i;
		i++;
	}
	
	return -1;
}

/*
 * Returns filter name.
 */
const char *evf_filter_get_name(struct evf_filter *filter)
{
	return filter->name;
}

/*
 * Returns filter descriptions.
 */
const char *evf_filter_get_desc(struct evf_filter *filter)
{
	return filter->desc;
}

/*
 * Loads one filter
 */
struct evf_filter *evf_filter_load(const char *name, char *params,
                                   union evf_err *err)
{
	int i = getfilter(name);

	if (i == -1) {
		err->type        = evf_errpar;
		err->param.etype = evf_efname;
		err->param.name  = name;
		return NULL;
	}

	return evf_creat[i](params, err);
}

/*
 * Free filter.
 */
void *evf_filter_free(struct evf_filter *filter)
{
	void *priv = NULL;

	/* Not to segfault on NULL just like free() */
	if (filter == NULL)
		return NULL;

	/* call filter cleanup if necessary */
	if (filter->free != NULL)
		priv = filter->free(filter);
	
	free(filter);

	return priv;
}

/*
 * Free all filters in linked list.
 */
void *evf_filters_free(struct evf_filter *root)
{
	struct evf_filter *tmp = root, *del;
	void  *priv;

	while (tmp != NULL) {
		del = tmp;
		tmp = tmp->next;
		priv = evf_filter_free(del);
	}

	return priv;
}

/*
 * Returns last filter in linked list
 */
struct evf_filter *evf_filters_last(struct evf_filter *root)
{
	struct evf_filter *tmp;

	if (root == NULL)
		return NULL;

	for (tmp = root; tmp->next != NULL; tmp = tmp->next);

	return tmp;
}

/*
 * Merge two linked lists of filters.
 */
struct evf_filter *evf_filters_merge(struct evf_filter *root,
                                     struct evf_filter *filters)
{
	struct evf_filter *last;
	
	/* first one is empty */
	if (root == NULL)
		return filters;

	last       = evf_filters_last(root);
	last->next = filters;

	return root;
}

/*
 * Debug function, prints linked list of filters to stdout. 
 */
void evf_filters_print(struct evf_filter *root)
{
	struct evf_filter *tmp;

	for (tmp = root; tmp->next != NULL; tmp = tmp->next)
		printf("%s->", evf_filter_get_name(tmp));

	printf("%s\n", evf_filter_get_name(tmp));
}
