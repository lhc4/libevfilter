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

#include <strings.h>
#include <stdlib.h>
#include "key_parser.h"
#include "keys.h"

int keyparser_getkey(const char *name)
{
	int left  = 0;
	int right = key_count;
	int oleft, oright;
	int cmp;

	do {
		if (!(cmp = strcasecmp(name, key_names[(left+right)/2]))) {
			return key_values[(left+right)/2];
		}
		
		oleft  = left;
		oright = right;

		if (cmp > 0) {
			left = (left+right)/2;
		} else {
			right = (left+right)/2;
		}
	
	} while (left != oleft || right != oright);

	return -1;
}

const char *keyparser_getname(const int i)
{
	int j;

	for (j = 0; j < key_count; j++)
		if (key_values[j] == i)
			return key_names[j];

	return NULL;
}
