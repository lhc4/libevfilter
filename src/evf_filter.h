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
  
  Here is defined basics filter loading function that can load filter
  accordingly to name and parameters. 
  
  Also some basics functionality to destroy filters as well as api to
  get some filter parameters.

 */

#ifndef __EVF_FILTER_H__
#define __EVF_FILTER_H__

struct input_event;
struct evf_filter;
union evf_err;

/*
 * Returns filter name.
 */
const char *evf_filter_get_name(struct evf_filter *filter);

/*
 * Returns filter descriptions.
 */
const char *evf_filter_get_desc(struct evf_filter *filter);

/*
 * Load filter with name and pass it params.
 *
 * NOTE: The string params is changed during parsing.
 */
struct evf_filter *evf_filter_load(const char *name, char *params,
                                   union evf_err *err);

/*
 * Free filter.
 *
 * NOTE: Filter can have additional free helper, so it's generally bad idea
 *       to call just free(filter_ptr) even if it works most of the time.
 *
 * NOTE2: Function is returning void* from additional free helper. It's way to 
 *        return back data passed to the filter from app.
 *
 */
void *evf_filter_free(struct evf_filter *filter);

/*
 * Free linked list of filters. Returns void* data, from last one deleted
 * filter, that if you are not doing anything funny is commit and we get void
 * *data from user.
 */
void *evf_filters_free(struct evf_filter *root);

/*
 * Returns last filter in linked list.
 */
struct evf_filter *evf_filters_last(struct evf_filter *root);

/*
 * Merges two linked lists of filters together. Second list is appended to the
 * end of first one. Returned value is begining of the new list.
 */
struct evf_filter *evf_filters_merge(struct evf_filter *root,
                                     struct evf_filter *filters);

/*
 * Debug function, prints linked list of filters to stdout. 
 */
void evf_filters_print(struct evf_filter *root);

#endif /* __EVF_FILTER_H__ */
