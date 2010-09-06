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

   IO queue for traditional unix select() call (tested only on linux).

   Member in io queue consists of file descriptor and callback, that is called,
   when data on fd are ready.

 */

#ifndef __EVF_IO_QUEUE_H__
#define __EVF_IO_QUEUE_H__

#include <sys/select.h>

/*
 * Returns values for queue member callback.
 *
 * Use as bitfields, to automatically remove member from queue and close fd:
 *
 * return EVF_IO_QUEUE_REM | EVF_IO_QUEUE_CLOSE;
 */
#define EVF_IO_QUEUE_OK    0x00 /* read was succesfull, continue */
#define EVF_IO_QUEUE_REM   0x01 /* remove memb from select queue */
#define EVF_IO_QUEUE_CLOSE 0x02 /* close fd                      */
#define EVF_IO_QUEUE_DFREE 0x04 /* call free on void *priv       */

/*
 * Select queue member.
 *
 * They are saved in sorted linked list, so that
 * maximum is O(1) and other functions O(N), but
 * as we are checking select flags in O(N) anyway
 * there is no reason to make it faster.
 */
struct evf_io_queue_memb {
	int fd;
	int (*read)(struct evf_io_queue_memb *self);
	void *priv;

	struct evf_io_queue_memb *next;
};

struct evf_io_queue {
	unsigned int cnt;
	fd_set rfds;
	struct evf_io_queue_memb *root;
};

/*
 * Create and initalize new queue. Uses malloc, may
 * fail and return NULL.
 */
struct evf_io_queue *evf_io_queue_new(void);

/*
 * Destroy, you can pass EVF_IO_QUEUE_CLOSE and EVF_IO_QUEUE_DFREE as a flag.
 */
void evf_io_queue_destroy(struct evf_io_queue *queue, int flags);

/*
 * Wait for data on any file descriptor or timeout.
 */
int evf_io_queue_wait(struct evf_io_queue *queue, struct timeval *timeout);

/*
 * Insert fd, its read function and priv pointer into queue.
 *
 */
int evf_io_queue_add(struct evf_io_queue *queue, int fd,
                     int (*read)(struct evf_io_queue_memb *self), void *priv);

/*
 * Remove fd from queue, filedescriptor is not closed here!.
 */
void evf_io_queue_rem(struct evf_io_queue *queue, int fd);

/*
 * Returns number of members in the queue.
 */
unsigned int evf_io_queue_get_count(struct evf_io_queue *queue);

#endif /* __EVF_IO_QUEUE_H__ */
