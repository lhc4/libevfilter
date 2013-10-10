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
 * Copyright (C) 2008-2013:                                                   *
 *     Cyril Hrubis <metan@ucw.cz>                                            *
 *     Erik Hamera                                                            *
 *     Vilem Marsik                                                           *
 *                                                                            *
 ******************************************************************************/

#ifndef __EVF_FUNC_H__
#define __EVF_FUNC_H__

/*
 * Eats all white-spaces
 */
void evf_eat_spaces(char **str);

/*
 * Reads lines from config, removes coments and empty lines
 */
void evf_read_line_preprocess(FILE *config, char *buf, size_t buf_len);


#endif /* __EVF_FUNC_H__ */
