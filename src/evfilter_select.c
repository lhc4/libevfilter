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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "evfilter_select.h"

/*
#include <stdio.h>
#define DEBUG_PRINT(...) { printf("%s: %i: ", __FILE__, __LINE__); printf(__VA_ARGS__); }
*/

#define DEBUG_PRINT(...)

/*
 * Insert into sorted linked list
 */
static struct evf_select_memb *list_insert(struct evf_select_memb *root, struct evf_select_memb *memb)
{
	struct evf_select_memb *prev = NULL, *here = root;

	DEBUG_PRINT("Inserting into list\n");

	while (here != NULL && here->fd > memb->fd) {
		prev = here;
		here = here->next;
	}

	/* we are changing root */
	if (prev == NULL) {
		memb->next = root;
		return memb;
	}
	
	prev->next = memb;
	memb->next = here;

	return root;
}

/*
 * Remove from sorted linked list
 */
static struct evf_select_memb *list_delete(struct evf_select_memb *root, int fd)
{
	struct evf_select_memb *prev = NULL, *here = root;

	while (here != NULL && here->fd != fd) {
		prev = here;
		here = here->next;
	}
	
	/* wasn't found */
	if (here == NULL)
		return root;

	if (prev == NULL)
		root = root->next;
	else
		prev->next = here->next;

	free(here);

	return root;
}

struct evf_select_queue *evf_select_new(void)
{
	struct evf_select_queue *queue = malloc(sizeof(struct evf_select_queue));

	if (queue == NULL)
		return NULL;
	
	FD_ZERO(&queue->rfds);
	queue->root = NULL;
	queue->cnt  = 0;

	return queue;
}

int evf_select(struct evf_select_queue *queue)
{
	int ret, ret_read;
	struct evf_select_memb *here = queue->root;

	/* empty queue */
	if (queue->root == NULL) {
		usleep(500);
		return 0;
	}

	if ((ret = select(queue->root->fd + 1, &queue->rfds, NULL, NULL, NULL)) < 0)
		return ret;

	for (here = queue->root; here != NULL; here = here->next) {
		
		DEBUG_PRINT("Testing for data on fd %i.\n", here->fd);

		if (FD_ISSET(here->fd, &queue->rfds)) {
			
			ret_read = here->read(here);

			if (ret_read == EVF_SEL_OK)
				continue;

			if (ret_read & EVF_SEL_CLOSE)
				close(here->fd);

			if (ret_read & EVF_SEL_DFREE)
				free(here->data);

			if (ret_read & EVF_SEL_REM)
				queue->root = list_delete(queue->root, here->fd);

		} else
			FD_SET(here->fd, &queue->rfds);
	}

	return ret;
}

int evf_select_add(struct evf_select_queue *queue, int fd, int (*read)(struct evf_select_memb *self), void *data)
{
	struct evf_select_memb *memb = malloc(sizeof(struct evf_select_memb));

	DEBUG_PRINT("Inserting fd %i into queue.\n", fd);

	if (memb == NULL)
		return 0;

	memb->fd   = fd;
	memb->read = read;
	memb->data = data;

	FD_SET(fd, &queue->rfds);

	queue->root = list_insert(queue->root, memb);
	queue->cnt++;

	return 1;
}

void evf_select_rem(struct evf_select_queue *queue, int fd)
{
	if (FD_ISSET(fd, &queue->rfds)) {
		FD_CLR(fd, &queue->rfds);
		queue->root = list_delete(queue->root, fd);
		queue->cnt--;
	}
}

unsigned int evf_select_count(struct evf_select_queue *queue)
{
	return queue->cnt;
}
