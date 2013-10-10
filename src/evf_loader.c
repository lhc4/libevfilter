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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "evf_struct.h"
#include "evf_filter.h"
#include "evf_err.h"
#include "evf_loader.h"
#include "evf_msg.h"
#include "evf_func.h"

/*
 * Loads one line from configuration file and returns filter name.
 *
 * Valid combinations are:
 *
 * FilterName name
 * FilterName=name
 * FilterName =name
 * FilterName = name
 * filtername name
 *
 * And many more with whitespaces and crazy caps all around.
 */
static int get_filter_name(FILE *config, char *buf, size_t buf_len)
{
	char str_buf[4096];
	char *line = str_buf;

	fscanf(config, "%4096[^\n]\n", line);
	
	evf_eat_spaces(&line);

	if (strncasecmp("FilterName", line, 10))
		return -1;
	
	line += 11;

	evf_eat_spaces(&line);

	if (*line == '=')
		line++;

	evf_eat_spaces(&line);
	
	strncpy(buf, line, buf_len);
	buf[buf_len - 1] = '\0';
	
	return 0;
}

/*
 * Stores everything to buf until line with EndFilter comes.
 */
static int get_filter_params(FILE *config, char *buf, size_t buf_len)
{
	char line[4096];
	size_t len;

	buf[0] = '\0';
	
	for (;;) {
		fscanf(config, "%4096[^\n]\n", line);

		len = strlen(line);
	
		if (!strcasecmp(line, "EndFilter"))
			break;
		
		if (feof(config))
			return -2;

		/* copy line into buffer and add separator (' ') */
		if (buf_len > 0 && len < buf_len - 2) {
			strcpy(buf, line);
			buf[len] = ' ';
			buf[++len] = '\0';
			buf     += len;
			buf_len -= len;
		}
		//TODO else: line too long
	}

	return 0;
}

/*
 * Takes file and returns pointer to linked list of filters
 * defined to load from this file or NULL and error condition
 * in err.
 */
struct evf_filter *evf_load_filters(const char *path, union evf_err *err)
{
	FILE *config = fopen(path, "r");
	char filter[256];
	char params[4096];
	struct evf_filter *filters = NULL, *last_filter, *tmp;
	int count=1;

	if (config == NULL) {
		//TODO: context to err
		err->type = evf_errno;
		err->err_no.err_no = errno;
		return NULL;
	}

	do  {
		if (get_filter_name(config, filter, 256) != 0) {
			//TOOD: syntax error, expecting FilterName, fill err
			printf("evf_load_filter: get_filter_name ERR\n");
			goto err;
		}
		
		if (get_filter_params(config, params, 4096) != 0) {
			//TODO: eof while looking for EndFilter, fill err
			printf("evf_load_filter: get_filter_params ERR\n");
			goto err;
		}
		
		//printf("FilterName ***%s***\n Params ***%s***\n",
		//       filter, params);
		
		tmp = evf_filter_load(filter, params, err);
		count ++;
		
		/*
		 * Error loading filter, err is filled from evf_load_filter.
		 */
		if (tmp == NULL)
			goto err;
		
		/*
		 * Add filter into linked list
		 */
		/* we are inserting to the end */
		tmp->next = NULL;
		
		/* do we have empty list? */
		if (filters == NULL) {
			filters           = tmp;
			last_filter       = tmp;
		} else {
			last_filter->next = tmp;
			last_filter       = tmp;
		}

	} while (!feof(config));
	
	evf_msg( EVF_INFO, "Loaded %i filters.", count );
	err->type = evf_ok;
	fclose(config);
	return filters;
err:
	evf_filters_free(filters);	
	fclose(config);
	return NULL;
}

/*
 * Just wrapper for evf_load_filters.
 */
struct evf_filter *evf_load_filters_compose(const char *path, const char *file,
                                            union evf_err *err)
{
	char *str_buf = malloc(strlen(path) + strlen(file) + 1);
	struct evf_filter *tmp;

	if (str_buf == NULL) {
		err->type = evf_errno;
		err->err_no.err_no = errno;
		return NULL;
	}

	strcpy(str_buf, path);
	strcpy(str_buf + strlen(path), file);

	tmp = evf_load_filters(str_buf, err);
	free(str_buf);
	
	return tmp;
}
