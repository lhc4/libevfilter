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
  
  This is catch all include file. After including this, you can use all functions
  that are meant to be used from outside this library.

 */

#ifndef __EVFILTER_H__
#define __EVFILTER_H__

/*
 * Basic linux input interface.
 */
#include <linux/input.h>

/*  Error reporting, functions to translate error codes to error messages. */
#include "evf_err.h"

/*
 * Basic filter interface. Can load filters, destroy them, get parameters.
 *
 * NOTE: Most of the interface are low level calls, do not use unless you know
 *       what are you doing.
 */
#include "evf_filter.h"

/* Load filters accroding to config file and returns handle to it. */
#include "evf_loader.h"

/*
 * Implements profiles. Profile is directory with special file that contains rules
 * and many more files with informations witch filters to load.
 */
#include "evf_profile.h"

/*
 * Evfilter line. High level interface, you can call just create line and
 * library looks for system wide profiles, loads filter and many more. This is
 * application preffered interface.
 */
#include "evf_line.h"

/* IO queue for file descriptor build on select() call */
#include "evf_io_queue.h"

/* Hotplug implementation. */
#include "evf_hotplug.h"

#endif /* __EVFILTER_H__ */
