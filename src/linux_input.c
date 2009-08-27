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

#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "linux_input.h"
#include "keyparser/key_parser.h"

int evf_input_get_version(int fd, int *version)
{
	if (ioctl(fd, EVIOCGVERSION, version))
		return -1;
	
	return 0;
}

/*
 * Okay, input layer in kernel uses copy_to_user kernel
 * macro to send us this string and this macro returns
 * number of bytes that was copied. So values <= 0 are
 * considered error conditions.
 */
int evf_input_get_name(int fd, char *buf, size_t buf_len)
{
	int ret;

	if ((ret = ioctl(fd, EVIOCGNAME(buf_len), buf)) <= 0)
		return -1;

	return ret;
}

/*
 * Same as abowe.
 */
int evf_input_get_phys(int fd, char *buf, size_t buf_len)
{
	int ret;

	if ((ret = ioctl(fd, EVIOCGPHYS(buf_len), buf)) <= 0)
		return -1;

	return ret;
}

/*
 * We compare major and minor number here.
 */
int evf_input_compare(int fd, const char *path)
{
	struct stat st1, st2;

	if (fstat(fd, &st1) == -1)
		return -1;

	if (stat(path, &st2) == -1)
		return -1;

//	printf("st_dev %ix%i %ix%i\n", minor(st1.st_rdev), major(st1.st_rdev), minor(st2.st_rdev), major(st2.st_rdev));

	return st1.st_rdev == st2.st_rdev;
}

/*
 * Pretty print for struct input_event.
 */
static void print_key(FILE *file, const char *prefix, struct input_event *ev)
{
	fprintf(file, "%sev->code:  %s (%i)\n", prefix, keyparser_getname(ev->code), ev->code);

	switch (ev->value) {
		case 0:
			fprintf(file, "%sev->value: KEY_UP     (0)\n", prefix);
		break;
		case 1: 
			fprintf(file, "%sev->value: KEY_DOWN   (1)\n", prefix);
		break;
		case 2:
			fprintf(file, "%sev->value: KEY_REPEAT (2)\n", prefix);
		break;
		default:
			fprintf(file, "%sev->value: UNKNOWN (%i)\n", prefix, ev->value);
	}
}

static char *rel_ev[10] = {"REL_X     ", "REL_Y     ", "REL_Z     ", "REL_RX    ", "REL_RY    ", 
                           "REL_RZ    ", "REL_HWHEEL", "REL_DIAL  ", "REL_WHEEL ", "REL_MISC  ", };

static void print_rel(FILE *file, const char *prefix, struct input_event *ev)
{
	if (ev->code < 11)
		fprintf(file, "%sev->code:  %s (%i)\n", prefix, rel_ev[ev->code], ev->code);
	else
		fprintf(file, "%sev->code:  UNKNOWN (%i)\n", prefix, ev->code);
	
	fprintf(file, "%sev->value: %i\n", prefix, ev->value);
}

static void print_abs(FILE* file, const char *prefix, struct input_event *ev)
{
	(void) ev;
	(void) prefix;

	fprintf(file, "TODO\n");
}


static char *ev_type[] = { "EV_SYN    ", "EV_KEY    ", "EV_REL    ", "EV_ABS    ", "EV_MSC    ", "EV_SW     ", 
                           "EV_LED    ", "EV_SND    ", "EV_REP    ", "EV_FF     ", "EV_PWR    ", "EV_FF_STAT", };

void evf_input_print(FILE *file, const char *prefix, struct input_event *ev)
{
	printf("%sev->type:  ", prefix);

	if (ev->type < 13)
		fprintf(file, "%s\n", ev_type[ev->type]);
	else
		fprintf(file, "UNKNOWN %i\n", ev->type);

	switch (ev->type) {
		case EV_KEY:
			print_key(file, prefix, ev);
		break;
		case EV_REL:
			print_rel(file, prefix, ev);
		break;
		case EV_ABS:
			print_abs(file, prefix, ev);
		break;
	}
}
