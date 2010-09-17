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
#include <syslog.h>
#include <stdarg.h>

#include "evfd_msg.h"

static char *evfd_process_name;

static char *evfd_msg_t_names[] = {
	"ERROR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG",
};

static int evfd_msg_t_levels[] = {
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG,
};

/* messages verbosity */
static enum evfd_msg_t evfd_verbosity = EVFD_NOTICE;

/* messages sinks */
static bool evfd_stderr = true;
static bool evfd_syslog = false;

void evfd_msg_init(char *process_name)
{
	evfd_process_name = process_name;
}

static void evfd_msg_stderr(enum evfd_msg_t type, const char *fmt, va_list va)
{
	char *type_name = "UNKNOWN";
	
	if (type <= EVFD_MAX)
		type_name = evfd_msg_t_names[type];

	fprintf(stderr, "%s: %s: ", type_name, evfd_process_name);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
}

static void evfd_msg_syslog(enum evfd_msg_t type, const char *fmt, va_list va)
{
	int level = LOG_ERR;
	
	openlog(evfd_process_name, LOG_PID, LOG_DAEMON);

	if (type <= EVFD_MAX)
		level = evfd_msg_t_levels[type];
	else
		syslog(LOG_DEBUG, "invalid log type, probably bug in %s.",
		       evfd_process_name);

	vsyslog(level, fmt, va);

	closelog();
}

void evfd_msg(enum evfd_msg_t type, const char *fmt, ...)
{
	va_list va;

	/* not enough verbosity to print message */
	if (type > evfd_verbosity)
		return;

	if (evfd_stderr) {
		va_start(va, fmt);
		evfd_msg_stderr(type, fmt, va);
		va_end(va);
	}

	if (evfd_syslog) {
		va_start(va, fmt);
		evfd_msg_syslog(type, fmt, va);
		va_end(va);
	}
}

void evfd_msg_verbosity_set(enum evfd_msg_t level)
{
	evfd_verbosity = level;
}

void evfd_msg_output(enum evfd_msg_out output, bool on_off)
{

	switch (output) {
		case EVFD_STDERR:
			evfd_stderr = on_off;
		break;
		case EVFD_SYSLOG:
			evfd_syslog = on_off;
		break;
	}

	return;
}

void evfd_msg_exit(void)
{
	return;
}
