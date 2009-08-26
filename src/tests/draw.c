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
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "evfilter.h"

#define X_RES 900
#define Y_RES 500
#define MAX_INPUTS 20

static SDL_Surface *scr;

int init_sdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 0;
	}

	scr = SDL_SetVideoMode(X_RES, Y_RES, 32, SDL_HWSURFACE);
	SDL_WM_SetCaption("Draw", "Draw");

	return 1;
}

void draw(unsigned int x, unsigned int y, uint32_t color)
{
		pixelColor(scr, x, y, color); 
		SDL_UpdateRect(scr, x-1, y-1, 2, 2);
}

/*
 * My internal structure
 */
struct input_device {
	uint16_t x;
	uint16_t y;

	int id;
};

/*
 * This is callback function, we get all events here.
 */
void commit(struct input_event *ev, void *data)
{
	struct input_device *dev = (struct input_device*) data;

	switch (ev->type) {
		case EV_REL:
			switch (ev->code) {
				case REL_X:
					dev->x += ev->value;
				break;
				case REL_Y:
					dev->y += ev->value;
				break;
			}
		break;
		case EV_SYN:
			if (dev->id == 1)
				draw(dev->x, dev->y, 0xffffffff);
			else
				draw(dev->x, dev->y, 0xffff00ff);
		break;
		default:
		break;
	}
}

int main(int argc, char *argv[])
{
	struct evf_line *line[10];
	struct input_device dev[10];
	int i, used = 0, max_fd = -1;
	fd_set rfds;
	union evf_err err;

	if (argc > MAX_INPUTS) {
		fprintf(stderr, "Maximal number of inputs is %i.\n", MAX_INPUTS);
		return 1;
	}

	if (!init_sdl()) {
		fprintf(stderr, "SDL init failed\n");
		return 1;
	}

	/* Create input lines */
	for (i = 1; i < argc; i++) {
		printf("Opening device `%s' ... ", argv[i]);

		line[used] = evf_line_create(argv[i], commit, &dev[used], 20, &err);

		if (line[used] == NULL) {
			printf("failed\n");
			evf_err_print(&err);
		} else {
			printf("ok\n");
			dev[used].x  = X_RES/2;
			dev[used].y  = Y_RES/2;
			dev[used].id = used + 1;
			used++;
		}
	}

	if (used == 0) {
		fprintf(stderr, "Cannot initalize any input devices!\n");
		return 1;
	}
	
	/* choose maximal fd for select */
	for (i = 0; i < used; i++) {
		if (max_fd < line[i]->fd)
			max_fd = line[i]->fd;
	}

	/* use select to wait for data on any input device */
	for (;;) {
		FD_ZERO(&rfds);	
		
		for (i = 0; i < used; i++)
			FD_SET(line[i]->fd, &rfds);

		if (select(max_fd+1, &rfds, NULL, NULL, NULL) < 0) {
			fprintf(stderr, "select(): %s", strerror(errno));
			return 1;
		}
		
		for (i = 0; i < used; i++)
			if (FD_ISSET(line[i]->fd, &rfds)) {
				evf_line_process(line[i]);
			}
	}

	return 0;
}
