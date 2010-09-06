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

#include <stdio.h>
#include <string.h>
#include "evfilter.h"

#define LINE_SIZE 70

static void do_padding(char *c)
{
	int len = strlen(c);
	int i;

	for (i = 0; i < LINE_SIZE-len; i++)
		fprintf(stderr, ".");
}

#define RESULT(x, str) fprintf(stderr, "%s ", str); \
                          do_padding(str); \
                          if(x) \
                          	fprintf(stderr, " OK\n"); \
                          else \
                          	fprintf(stderr, " FAILED\n"); \

#define RAW_MSG printf("  Error message from libevfilter: ");

/*
 * Hope nobody will ever create filter name xxx42xxx
 */
void invalid_filter_name(void)
{
	union evf_err err;

	RESULT(evf_filter_load("xxx42xxx", "", &err) == NULL && err.type == evf_errpar && err.param.etype == evf_efname, "Invalid filter name");

	RAW_MSG
	evf_err_print(&err);
}

/*
 * Empty param string for filter with parameters
 */
void no_params_at_all(void)
{
	union evf_err err;

	RESULT(evf_filter_load("dump", "", &err) == NULL && err.type == evf_errpar && err.param.etype == evf_emissing, "No params at all");
	
	RAW_MSG
	evf_err_print(&err);
}

/*
 * Invalid parameter name
 */
void invalid_parameter_name(void)
{
	union evf_err err;
	char  params[] = "gugu = 42";

	RESULT(evf_filter_load("dump", params, &err) == NULL && err.type == evf_errpar && err.param.etype == evf_epname, "Invalid parameter name");

	RAW_MSG
	evf_err_print(&err);
}

/*
 * Missing parameter, dump expects two parameters, prefix=String file=Path
 */
void missing_parameter(void)
{
	union evf_err err;
	char  params[] = "prefix='debug'";

	RESULT(evf_filter_load("dump", params, &err) == NULL && err.type == evf_errpar && err.param.etype == evf_emissing, "Missing parameter");

	RAW_MSG
	evf_err_print(&err);
}

/*
 * Correct load, file loader has two special file names stdout and stderr
 */
void correct_load(void)
{
	struct evf_filter *evf;
	union  evf_err err;
	char   params[] = "prefix='debug : ' file = stdout";

	RESULT((evf = evf_filter_load("dump", params, &err)) != NULL, "Correct module load");

	if (evf == NULL)
		evf_err_print(&err);

	evf_filter_free(evf);
}

int main(void)
{
	
	invalid_filter_name();
	no_params_at_all();
	invalid_parameter_name();
	missing_parameter();
	correct_load();

	return 0;
}
