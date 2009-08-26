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

/*

  Here is implemented hotplug support for evfilter library.

  Currently the implementation uses inofity framework.

 */

#ifndef __EVFILTER_HOTPLUG_H__
#define __EVFILTER_HOTPLUG_H__

/*
 * Register callback function and reset list of known devices. 
 */
int evf_hotplug_init(void (*device_plugged)(const char *dev), void (*device_unplugged)(const char *dev));

/*
 * Parse /proc/bus/input/devices and call callbacks. Can return -1 if
 * parsing has failed. Returns 0 on success.
 */
int evf_hotplug_rescan(void);

/*
 * Exit the hotplug.
 */
void evf_hotplug_exit(void);

#endif /* __EVFILTER_HOTPLUG_H__ */
