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
  
  Constants.

 */

#ifndef __EVFD_H__
#define __EVFD_H__

static char *evfd_help = 
	"\n.................. -=[ EVFD ]=- .................\n"
	"                                                 \n"
	"  -- daemon for modifiying input events          \n"
	"                                                 \n"
	" parameters                                      \n"
	" ----------                                      \n"
	" -v  verbose output                              \n"
	" -d  do not dameonize                            \n"
	" -h  print this help                             \n"
	"                                                 \n"
	" description                                     \n"
	" -----------                                     \n"
	" Evfd looks for evfilter system configuration in \n"
	" /etc/evfilter/ and creates virtual input devices\n"
	" and routes filtered events from grabbed devices \n"
	" into them.                                      \n"
	"                                                 \n"
	" written by && bugs to                           \n"
	" ---------------------                           \n"
	" Cyril Hrubis metan@ucw.cz                       \n\n";

#endif /* __EVFD_H__ */
