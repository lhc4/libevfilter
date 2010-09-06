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

#include <string.h>
#include <stdio.h>

#include "evf_err.h"
#include "evf_param.h"

//#define DPRINT(...) { fprintf(stderr, "%s: %i: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define DPRINT(...)

/*
 * Print error message according to union evf_err
 * TODO: convert error message to string
 */
void evf_err_print(union evf_err *err)
{
	if (err->type == evf_ok) {
		printf("Operation was succesfull.\n");
		return;
	}

	if (err->type == evf_errno) {
		printf("Errno: %s\n", strerror(err->err_no.err_no));
		return;
	}

	if (err->type != evf_errpar) {
		printf("Internal Error: Invalid error type (%i) !\n",
		       err->type);
		return;
	}

	switch (err->param.etype) {
		case evf_emissing:
			printf("Parameter `%s' is missing.\n", err->param.name);
		break;
		case evf_epname:
			printf("Invalid parameter `%s'.\n", err->param.name);
		break;
		case evf_efname:
			printf("Invalid filter name `%s'.\n", err->param.name);
		break;
		case evf_einval:
			printf("Invalid value `%s' for parameter `%s'. "
			       "Expected %s.\n",
			       err->param.value, err->param.name, 
			       evf_get_type_name(err->param.ptype));
		break;
		case evf_erange:
			//TODO: print range
			printf("Parameter `%s' out of range.\n",
			       err->param.name);
		break;
		case evf_eredef:
			printf("Parameter `%s' redefined.\n", err->param.name);
		break;
		default:
			printf("Internal Error: Invalid error number (%i) !\n",
			       err->param.ptype);
	}
}
