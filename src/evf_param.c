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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>

#include "key_parser.h"

#include "evf_param.h"
#include "evf_err.h"

//#define DPRINT(...) { fprintf(stderr, "%s: %i: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define DPRINT(...)

static const char *evf_type_names[] = {
	"Key", "Keys", "Integer", "Float",
	"String", "Path to file", "Bool"
};

/* Default limits */
static struct evf_lim_int evfilter_int_limits = {INT_MIN, INT_MAX};

/* 
 * Evt_param_t <-> string translation.
 */
const char *evf_get_type_name(enum evf_param_t type)
{
	return evf_type_names[type];
}

static int evfilter_load_float(union evf_err *err, char *str, float *val,
                               struct evf_param *param)
{
	float fval;
	char *ptr;

	fval = strtof(str, &ptr);

	if (*ptr != '\0') {
		err->type        = evf_errpar;
		err->param.etype = evf_einval;
		err->param.name  = param->name;	
		err->param.value = str;
		err->param.ptype = evf_float;

		return -1;
	}

	*val = fval;

	return 0;
}

/*
 * Converts string to integer and checks limits.
 */
static int evfilter_load_int(union evf_err *err, char *str, int *val,
                             struct evf_param *param)
{
	long int lval;
	int      ival;
	char    *ptr;

	lval = strtol(str, &ptr, 0);

	/*
	 * Check that whole string was used in conversion 
	 */
	if (*ptr != '\0') {
		err->type        = evf_errpar;
		err->param.etype = evf_einval;
		err->param.name  = param->name;	
		err->param.value = str;
		err->param.ptype = evf_int;

		return -1;
	}

	/*
	 * Check for range
	 */
	if (lval > INT_MAX || lval < INT_MIN) {
		
		err->type        = evf_errpar;
		err->param.etype = evf_erange;
		err->param.name  = param->name;

		if (param->lim)
			err->param.lim = param->lim;
		else
			err->param.lim = &evfilter_int_limits;

		return -1;
	}

	ival = (int) lval;

	/*
	 * Check for limits if needed
	 */
	if (param->lim != NULL) {
		
		struct evf_lim_int *lim = param->lim;

		if (ival > lim->max || ival < lim->min) {
			err->type        = evf_errpar;
			err->param.etype = evf_erange;
			err->param.name  = param->name;
			err->param.lim   = lim;
			
			return -1;
		}
	}

	*val = ival;
	
	return 0;
}

/*
 * Opens file according to path.
 *
 * There are two special names:
 *
 * stdout == standart out
 * stderr == standart err
 */
static int evfilter_load_file(union evf_err *err, char *str, FILE **f,
                              struct evf_param *param)
{
	if (!strcasecmp(str, "stdout")) {
		*f = stdout;
		return 0;
	}

	if (!strcasecmp(str, "stderr")) {
		*f = stderr;
		return 0;
	}

	*f = fopen(str, "rw");

	if (*f == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
		
		return -1;
	}
	
	return 0;
}

/*
 * Loads bool.
 *
 * These examples are correct:
 *
 * True
 * Yes
 * On
 *
 * False
 * No
 * Off
 */
static int evfilter_load_bool(union evf_err *err, char *str, int *val,
                              struct evf_param *param)
{
	if (!strcasecmp(str, "True") || 
	    !strcasecmp(str, "Yes")  ||
	    !strcasecmp(str, "On")) {
		*val = 1;
		return 0;
	}

	if (!strcasecmp(str, "False") ||
	    !strcasecmp(str, "No")    ||
	    !strcasecmp(str, "Off")) {
		*val = 0;
		return 0;
	}
		
	err->type        = evf_errpar;
	err->param.etype = evf_einval;
	err->param.name  = param->name;
	err->param.value = str;
	err->param.ptype = evf_bool;

	return -1;
}

/* 
 * Eats all white-spaces
 */
static void evfilter_eat_spaces(char **str)
{
	while (isspace(**str)) (*str)++;
}

/*
 * Converts string into int key.
 */
static int evfilter_load_key(char *str, int *key)
{
	int pkey = keyparser_getkey(str);

	if (pkey == -1)
		return -1;

	*key = pkey;

	return 0;
}

/*
 * String int event type.
 */
static char *evtypes[] = {
	"EventSyn",
	"EventKey",
	"EventRel",
	"EventAbs",
	"EventMsc",
	NULL,
};

static int evfilter_load_evtype(char *str, int *evtype)
{
	int i;

	for (i = 0; evtypes[i] != NULL; i++)
		if (!strcasecmp(evtypes[i], str)) {
			*evtype = i;
			return 0;
		}

	return -1;
}

/*
 * Prepares name from string. It's string ended with white-spaces or '='.
 *
 * Following examples are correct:
 *
 * name param
 * name=param
 * name= param
 * name = param
 */
static char *evfilter_get_name(char **str)
{
	char *name;
	
	evfilter_eat_spaces(str);
	name = *str;

	/*
	 * Wait for white space or '='
	 */
	for (; **str != '\0'; (*str)++) {
		if (isspace(**str) || **str == '=') {
			**str = '\0';
			(*str)++;
			break;
		}
	}

	evfilter_eat_spaces(str);
	
	if (**str == '=')
		(*str)++;

	DPRINT("name `%s'", name)

	return name;
}

/*
 * Prepares value from string.
 *
 * Every parametr can be enclosed in " or '. If it's so, it can contain
 * white-spaces.  If parameter is enclosed with ", there can't be any " in
 * string and vice versa.
 *
 * Following examples are correct:
 *
 * this_is_param
 * "this_is_param"
 * 'this_is_param'
 * "this's param"
 * 'this is param'
 * 'this is "param"'
 */
static char *evfilter_get_value(char **str)
{
	char *param;
	char enc = 0;
	
	evfilter_eat_spaces(str);
	
	/*
	 * String is enclosed in " or '
	 */
	if (**str == '"' || **str == '\'') {
		enc = **str;
		(*str)++;
	}
	
	param = *str;

	/*
	 * Wait for the white-space or ' or ".
	 */
	for (; **str != '\0'; (*str)++) {
		
		if (enc) {
			if (**str == enc) {
				**str = '\0';
				(*str)++;
				break;
			}
		} else {
			if (isspace(**str)) {
				**str = '\0';
				(*str)++;
				break;
			}
		}
	}

	DPRINT("value `%s'", param)

	return param;
}

/*
 * Find possition in field of struct evf_param[].
 */
static int evfilter_find_possition(char *name, struct evf_param params[])
{
	int res;
	unsigned int i;

	for (i = 0; params[i].name != NULL; i++) {
		
		res = strcasecmp(params[i].name, name);
	
		if (res == 0)
			return i;
	}

	return -1;
}

/*
 * Just counts number of NULL terminated structs
 */
static int evfilter_count_params(struct evf_param params[])
{
	int i;

	for (i = 0; params[i].name != NULL; i++);

	return i;
}

/*
 * All filter parameters are stored in string cfg in format
 * parameter_name = parameter
 */
int evf_load_params(union evf_err *err, char *cfg, struct evf_param params[],
                    ...)
{
	va_list list;
	int n = evfilter_count_params(params);
	char *values[n];
	char *name;
	int i;

	/*
	 * Initalization
	 */
	for (i = 0; i < n; i++)
		values[i] = NULL;

	/*
	 * First parse string cfg
	 */
	while (*cfg != '\0') {
		
		name = evfilter_get_name(&cfg);
		i = evfilter_find_possition(name, params);
		
		DPRINT("possition %i", i);

		/* invalid parameter name */
		if (i < 0) {
			err->type        = evf_errpar;
			err->param.etype = evf_epname;
			err->param.name  = name;

			return -1;
		}

		/* parameter redefined */
		if (values[i] != NULL) {
			err->type        = evf_errpar;
			err->param.etype = evf_eredef;
			err->param.name  = name;

			return -1;
		}
		
		values[i] = evfilter_get_value(&cfg);
	}
	
	/*
	 * Then load all parameters 
	 */
	va_start(list, params);

	for (i = 0; params[i].name != NULL; i++) {

		/* parameter missing */
		if (values[i] == NULL) {
			err->type        = evf_errpar;
			err->param.etype = evf_emissing;
			err->param.name  = params[i].name;
			
			return -1;
		}

		switch (params[i].type) {
			case evf_int:
				if (evfilter_load_int(err, values[i],
				    va_arg(list, int*), &params[i]))
					return -1;
			break;
			case evf_evtype:
				if (evfilter_load_evtype(values[i],
				    va_arg(list, int*))) {
					err->type        = evf_errpar;
					err->param.etype = evf_einval;
					err->param.ptype = evf_evtype;
					err->param.name  = params[i].name;
					err->param.value = values[i];

					return -1;
				}
			break;
			case evf_key:
				if (evfilter_load_key(values[i],
				    va_arg(list, int*))) {
					err->type        = evf_errpar;
					err->param.etype = evf_einval;
					err->param.ptype = evf_key;
					err->param.name  = params[i].name;
					err->param.value = values[i];

					return -1;
				}
			break;
			case evf_file:
				if (evfilter_load_file(err, values[i],
				    va_arg(list, FILE**), &params[i]))
					return -1;
			break;
			case evf_str:
				*va_arg(list, char**) = values[i];
			break;
			case evf_bool:
				if (evfilter_load_bool(err, values[i],
				    va_arg(list, int*), &params[i]))
					return -1;
			break;
			case evf_float:
				if (evfilter_load_float(err, values[i],
				    va_arg(list, float*), &params[i]))
					return -1;
			break;
		}
	}

	va_end(list);

	return 0;
}
