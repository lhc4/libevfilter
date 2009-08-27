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

   Here is implemented queue for traditional linux select() call.

   In fact, you create queue and add all filedescriptors with callback there.

 */

#ifndef __EVFILTER_SELECT_H__
#define __EVFILTER_SELECT_H__

#include <sys/select.h>

/*
 * Returns values for read() callback from evf_select_memb.
 *
 * Use as bitfields, to remove member from queue and close fd:
 *
 * return EVF_SEL_REM | EVF_SEL_CLOSE;
 */
#define EVF_SEL_OK    0x00 /* read was succesfull, continue */
#define EVF_SEL_REM   0x01 /* remove memb from select queue */
#define EVF_SEL_CLOSE 0x02 /* close fd                      */
#define EVF_SEL_DFREE 0x04 /* call free on void *data       */

struct evf_select_queue;

/*
 * Select queue member.
 *
 * They are saved in sorted linked list, so that
 * maximum is O(1) and other functions O(N), but
 * as we are checking select flags in O(N) anyway
 * there is no reason to make it faster.
 */
struct evf_select_memb {
	int fd;
	int (*read)(struct evf_select_memb *self);
	void *data;
	
	struct evf_select_memb *next;
};

struct evf_select_queue {
	unsigned int cnt;
	fd_set rfds;
	struct evf_select_memb *root;
};

/*
 * Create and initalize new queue. Uses malloc, may
 * fail and return NULL.
 */
struct evf_select_queue *evf_select_new(void);

/*
 * Destroy
 */
void evf_select_destroy(struct evf_select_queue *queue);

/*
 * Returns true for empty queue.
 */
int evf_select_empty(struct evf_select_queue *queue);

/*
 * Do a select() on file descriptors in queue.
 */
int evf_select(struct evf_select_queue *queue);

/*
 * Insert fd, its read function and data into queue.
 *
 * Returns 
 */
int evf_select_add(struct evf_select_queue *queue, int fd, int (*read)(struct evf_select_memb *self), void *data);

/*
 * Remove fd from queue, filedescriptor is not closed here!.
 */
void evf_select_rem(struct evf_select_queue *queue, int fd);

/*
 * Returns number of members in the queue.
 */
unsigned int evf_select_count(struct evf_select_queue *queue);

#endif /* __EVFILTER_SELECT_H__ */
