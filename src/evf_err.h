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
 
  Here is defined basic error reporting functionality.

 */

#ifndef __EVF_ERR_H__
#define __EVF_ERR_H__

#include "evf_param.h"

/*
 * Error types.
 */
enum evf_err_t {
	evf_ok,      /* all ok                                     */
	evf_errno,   /* errno from linux call                      */
	evf_errpar,  /* parse error, some parameters was incorrect */
};

/*
 * Parse error types.
 */
enum evf_err_par_t {
	evf_efname,   /* ivalid filter name; fills: err and name             */
	evf_epname,   /* ivalid parameter name; fills: err and name          */
	evf_emissing, /* parameter missing; fills: err and name              */
	evf_einval,   /* invalid value; fills: value, name, err and type     */
	evf_erange,   /* value out of range; fills: value, name, err and lim */
	evf_eredef,   /* value redefined; fills: value, name and err         */
};

/*
 * Errno from underlying linux call.
 *
 * Currently EMALLOC and errors from fopen()
 */
struct evf_err_errno {
	enum evf_err_t type;
	int err_no;
};

/*
 * Contains errors caused parse configuration errors.
 */
struct evf_err_param {
	enum evf_err_t type;
	enum evf_err_par_t etype;
	enum evf_param_t ptype;
	const char *name;
	const char *value;
	void *lim;
};

/*
 * This is the real err structure, use this one.
 */
union evf_err {
	enum evf_err_t type;
	struct evf_err_errno err_no;
	struct evf_err_param param;
};

/*
 * Prints error to stdout.
 */
void evf_err_print(union evf_err *err);

#endif /* __EVF_ERR_H__ */
