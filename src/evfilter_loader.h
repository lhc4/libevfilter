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
  
  Evfilter loader gives you simple api to load all filters from one
  configuration file.

  The file consist of several blocks like this:

  Filter = FilterName
	param1 = value
	param2 = value
  EndFilter

  Every content between "Filter = FilterName" and "EndFilter" is simply passed
  to evf_load_filter().

 */

#ifndef __EVFILTER_LOADER_H__
#define __EVFILTER_LOADER_H__

#include "evfilter.h"
#include "evfilter_err.h"

/*
 * Create filter line from configuration file.
 *
 * On succesfull operation pointer to filter line is returned. NULL is valid
 * value for empty file, so don't forget to check err in this case unless you
 * say empty file is invalid configuration.
 */
struct evf_filter *evf_load_filters(const char *path, union evf_err *err);

/*
 * Dtto, but compose path from path and filename.
 */
struct evf_filter *evf_load_filters_compose(const char *path, const char *file, union evf_err *err);


#endif /* __EVFILTER_LOADER_H__ */
