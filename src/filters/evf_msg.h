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
  
  Error messages redirecting.

 */

#ifndef __EVF_MSG_H__
#define __EVF_MSG_H__

#include <stdbool.h>

enum evf_msg_t {
	EVF_ERR,
	EVF_WARN,
	EVF_NOTICE,
	EVF_INFO,
	EVF_DEBUG,
	EVF_MAX = EVF_DEBUG,
};

enum evf_msg_out {
	EVF_STDERR,
	EVF_SYSLOG,
};

/*
 * Initalize messages.
 */
void evf_msg_init(char *process_name);

/*
 * Prints message.
 */
void evf_msg(enum evf_msg_t type, const char *fmt, ...);

/*
 * Turn on/off output.
 */
void evf_msg_output(enum evf_msg_out output, bool on_off);

/*
 * Set verbosity level.
 */
void evf_msg_verbosity_set(enum evf_msg_t level);

/*
 * Deinitalize messages.
 */
void evf_msg_exit(void);

#endif /* __EVF_MSG_H__ */
