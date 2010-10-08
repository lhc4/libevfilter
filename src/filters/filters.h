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

#ifndef __FILTERS_H__
#define __FILTERS_H__

struct input_event;
union evf_err;

/*
 * These are meant to be called directly from C source.
 *
 * WARNING: correctness of passed arguments is not checked!
 *          Anyway you've been warned Muhehehe....
 */
struct evf_filter *evf_commit_alloc(void (*commit)(struct input_event *ev,
                                    void *data), void *data);

struct evf_filter *evf_barrier_alloc(unsigned int history);

/*
 * You can pass parameters from useras params here.
 */
struct evf_filter* evf_dump_creat(char *params, union evf_err *err);
struct evf_filter* evf_no_repeat_creat(char *params, union evf_err *err);
struct evf_filter* evf_pressure_to_key_creat(char *params, union evf_err *err);
struct evf_filter* evf_speed_mod_rel_creat(char *params, union evf_err *err);
struct evf_filter* evf_barrier_creat(char *params, union evf_err *err);
struct evf_filter* evf_weighted_average_abs_creat(char *params,
                                                  union evf_err *err);
struct evf_filter* evf_scale_abs_creat(char *params, union evf_err *err);
struct evf_filter* evf_mirror_creat(char *params, union evf_err *err);
struct evf_filter* evf_rotate_creat(char *params, union evf_err *err);
struct evf_filter* evf_abs2rel_creat(char *params, union evf_err *err);
struct evf_filter* evf_btn2rel_creat(char *params, union evf_err *err);
struct evf_filter* evf_kalman_creat(char *params, union evf_err *err);
struct evf_filter* evf_rel2scroll_creat(char *params, union evf_err *err);

#endif /* __FILTERS_H__ */
