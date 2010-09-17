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

#ifndef __EVFD_MSG_H__
#define __EVFD_MSG_H__

#include <stdbool.h>

enum evfd_msg_t {
	EVFD_ERR,
	EVFD_WARN,
	EVFD_NOTICE,
	EVFD_INFO,
	EVFD_DEBUG,
	EVFD_MAX = EVFD_DEBUG,
};

enum evfd_msg_out {
	EVFD_STDERR,
	EVFD_SYSLOG,
};

/*
 * Initalize messages.
 */
void evfd_msg_init(char *process_name);

/*
 * Prints message.
 */
void evfd_msg(enum evfd_msg_t type, const char *fmt, ...);

/*
 * Turn on/off output.
 */
void evfd_msg_output(enum evfd_msg_out output, bool on_off);

/*
 * Set verbosity level.
 */
void evfd_msg_verbosity_set(enum evfd_msg_t type);

/*
 * Deinitalize messages.
 */
void evfd_msg_exit(void);

#endif /* __EVFD_MSG_H__ */
