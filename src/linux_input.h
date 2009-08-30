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

#ifndef __LINUX_INPUT_H__
#define __LINUX_INPUT_H__

#include <stdlib.h>
#include <stdio.h>

struct input_event;

/*
 * Returns input version, to print it use:
 *
 * printf("%d %d %d", version>>16, version>>8 & 0xff, version & 0xff);
 */
int evf_input_get_version(int fd, int *version);

/*
 * Returns up to buf_len characters of input device name.
 */
int evf_input_get_name(int fd, char *buf, size_t buf_len);

/*
 * Returns up to buf_len characters of input device phys.
 */
int evf_input_get_phys(int fd, char *buf, size_t buf_len);

/*
 * Compares minor an major number of fd and path.
 *
 * Returns:
 * -1 on error
 *  1 if numbers are the same
 *  0 if minor or major number are different
 */
int evf_input_compare(int fd, const char *path);

/*
 * Prints human readable decompostion of input_event
 * into file.
 */
void evf_input_print(FILE *file, const char *prefix, struct input_event *ev);

/*
 * Event type to string.
 */
const char *evf_input_type(struct input_event *ev);

/*
 * Event code to string.
 */
const char *evf_input_code(struct input_event *ev);

/*
 * Event value to string.
 */
const char *evf_input_value(struct input_event *ev);

#endif /* __LINUX_INPUT_H__ */
