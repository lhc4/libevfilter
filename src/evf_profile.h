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
 
  Profile in evfilter library is way to do either system wide configuration,
  eg. touchscreen calibration or specify some configuration for your own tool
  eg. slow down all relative pointers for console mouse daemon.

  Basically profile is directory with profile file that contains matching rules
  and some profile files where any such file contains list of modules to load.
  
  Profile file consist of several lines where each one defines one of following
  matching rules:
  
  Device /path/to/device               //we compare minor and major number here
  Name Input Hardware Name
  Phys /device/phys/from/kernel
  Bits Hardware capabilities
  File evfilter configuration to load

  See /proc/bus/input/devices for identifying your hardware.

  System wide configuration should live in /etc/evfilter/profile/profilerc or
  something close.

*/

#ifndef __EVF_PROFILE_H__
#define __EVF_PROFILE_H__

struct evf_filter;
union evf_err;

/*
 * Load system wide configuration.
 *
 * Takes file descriptor to input device, returns list of filters.
 *
 * You should allways check evf_err_t in err, because NULL here is 
 * correct return value.
 */
struct evf_filter *evf_load_system_profile(int fd, union evf_err *err);

/*
 * Same as above, but for user defined profile directory. 
 */
struct evf_filter *evf_load_profile(const char *path, int fd,
                                    union evf_err *err);


#endif /* __EVF_PROFILE_H__ */
