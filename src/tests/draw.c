/*****************************************************************************
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
  
  This is simple program using sdl as graphics driver and evfilter for input
  event filtering.

 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "evfilter.h"

#define X_RES 900
#define Y_RES 500

static SDL_Surface *scr;

int init_sdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 0;

	scr = SDL_SetVideoMode(X_RES, Y_RES, 32, SDL_HWSURFACE);
	SDL_WM_SetCaption("Draw", "Draw");

	return 1;
}

void draw(unsigned int x, unsigned int y, uint32_t color)
{
		pixelColor(scr, x, y, color); 
		SDL_UpdateRect(scr, x-1, y-1, 2, 2);
}

struct pointer {
	int x;
	int old_x;
	int y;
	int old_y;
	int color;
};

/*
 * Here we parse event as it came 
 */
static int read_event(struct evf_select_memb *self)
{
	struct input_event ev;
	struct pointer *dev = self->data;

	if (read(self->fd, &ev, sizeof(struct input_event)) > 0) {
		
		switch (ev.type) {
			case EV_REL:
				switch (ev.code) {
					case REL_X:
						dev->old_x = dev->x;
						dev->x += ev.value;
					break;
					case REL_Y:
						dev->old_y = dev->y;
						dev->y += ev.value;
					break;
				}
			break;
			case EV_SYN: {

				filledCircleColor(scr, dev->x, dev->y, 1, 0xff0000ff);
				lineColor(scr, dev->x, dev->y, dev->old_x, dev->old_y, dev->color);
				SDL_UpdateRect(scr, 0, 0, X_RES, Y_RES);
			} break;
		}
	} else {
		fprintf(stderr, "Device unplugged!\n");
		return EVF_SEL_REM | EVF_SEL_CLOSE |EVF_SEL_DFREE;
	}

	return EVF_SEL_OK;
}

static struct evf_select_queue *queue;

static int read_hotplug(struct evf_select_memb *self)
{
	evf_hotplug_rescan();
	
	return EVF_SEL_OK;
}

static int text_y = 0;

static void device_plugged(const char *dev)
{
	struct pointer *ptr;
	int fd;


	fprintf(stderr, "Device plugged!\n");

	fd  = open(dev, O_RDONLY);

	if (fd < -1) {
		fprintf(stderr, "Can't open %s: %s\n", dev, strerror(errno));
		return;
	}

	ptr = malloc(sizeof(struct pointer));

	if (ptr == NULL) {
		fprintf(stderr, "Can't allocate memory.\n");
		close(fd);
		return;
	}

	ptr->x = X_RES/2;
	ptr->y = Y_RES/2;
	ptr->old_x = X_RES/2;
	ptr->old_y = Y_RES/2;
	ptr->color = 0x000000ff | (random() % 0xff0000);

	evf_select_add(queue, fd, read_event, ptr);

	stringColor(scr, 10, 10 + text_y, dev, 0xff0000ff);
	SDL_UpdateRect(scr, 0, 0, X_RES, Y_RES);
	text_y+=10;
}

int main(void)
{
	union evf_err err;
	int fd;
	
	if (!init_sdl()) {
		fprintf(stderr, "SDL init failed\n");
		return 1;
	}
	
	queue = evf_select_new();

	if (queue == NULL) {
		fprintf(stderr, "Can't allocate select queue!\n");
		return 1;
	}

	if ((fd = evf_hotplug_init(device_plugged, NULL)) == -1) {
		fprintf(stderr, "Hotplug init failed: %s\n", strerror(errno));
		return 1;
	}

	evf_select_add(queue, fd, read_hotplug, NULL);


	for (;;)
		evf_select(queue);

	return 0;
}
