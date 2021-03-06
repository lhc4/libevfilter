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

  Here is implemented hotplug support for evfilter library.

  Currently the implementation uses inofity framework.

 */

#ifndef __EVF_HOTPLUG_H__
#define __EVF_HOTPLUG_H__

struct evf_io_queue;

/*
 * Register callback function and reset list of known devices. 
 */
int evf_hotplug_init(void (*device_plugged)(const char *dev),
                     void (*device_unplugged)(const char *dev));

/*
 * Initalize hotplug and add it into io_queue.
 */
int evf_hotplug_io_queue_init(struct evf_io_queue *queue);

/*
 * Parse /proc/bus/input/devices and call callbacks. Returns
 * number of hotplug events. Can return -1 if parsing has failed. 
 */
int evf_hotplug_rescan(void);

/*
 * Exit the hotplug.
 */
void evf_hotplug_exit(void);

#endif /* __EVF_HOTPLUG_H__ */
