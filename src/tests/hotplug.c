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
 
  This program is testing hotplug support just by printing hotplug events to
  stdout.

 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "evfilter.h"
#include "linux_input.h"

static void device_plugged(const char *dev)
{
	char buf[512];
	int fd;
	
	fd = open(dev, O_RDONLY);
	
	if (fd < 0)
		perror(dev);

	if (fd > 0) {
		evf_input_get_name(fd, buf, 512);
		close(fd);
	} else
		buf[0] = '\0';

	printf("--> Device   plugged at: %s (%s)\n", dev, buf);
}


static void device_unplugged(const char *dev)
{
	printf("<-- Device unplugged at: %s\n", dev);
}

int main(void)
{
	int fd;
	fd_set rfds;

	printf("Initalizing hotplug!\n");

	if ((fd = evf_hotplug_init(device_plugged, device_unplugged)) < 0) {
		perror("Error initalizing hotplug!");
		return 1;
	}

	FD_SET(fd, &rfds);

	while (select(fd + 1, &rfds, NULL, NULL, NULL) != -1) {
		
		if (FD_ISSET(fd, &rfds))
			evf_hotplug_rescan();
		
		FD_SET(fd, &rfds);
	}
	
	return 0;
}
