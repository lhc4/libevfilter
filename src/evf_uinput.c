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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>
#include <stdio.h>

#include "config.h"
#include "evf_uinput.h"

static unsigned long long input_counter = 0;

int evf_uinput_create(struct uinput_user_dev *ui_dev_info)
{
	int fd, i, ret;

	fd = open(EVFILTER_UINPUT_DEV_PATH, O_WRONLY);

	if (fd < 0)
		return -1;

	/* just enable all by default */
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_REL);
	ioctl(fd, UI_SET_RELBIT, REL_X);
	ioctl(fd, UI_SET_RELBIT, REL_Y);

	ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);

	for (i = 0; i < 256; i++)
		ioctl(fd, UI_SET_KEYBIT, i);

	ret = write(fd, ui_dev_info, sizeof (*ui_dev_info));

	if (ret != sizeof (*ui_dev_info)) {
		close(fd);
		return -2;
	}

	/* create uinput device */
	ret = ioctl(fd, UI_DEV_CREATE);

	if (ret != 0) {
		close(fd);
		return -3;
	}

	return fd;
}

void evf_uinput_destroy(int fd)
{
	ioctl(fd, UI_DEV_DESTROY);
	close(fd);
}
