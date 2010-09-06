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

#include <stdio.h>
#include "evfilter.h"

void load_dump(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "Prefix = 'dump:' File=stdout";

	evf = evf_filter_load("Dump", params, &err);

	if (evf == NULL) {
		printf("Dump               failed: \n");
		evf_err_print(&err);
	} else
		printf("Dump               success\n");

	evf_filter_free(evf);
}

void load_no_repeat(void)
{
	union evf_err err;
	struct evf_filter *evf;

	evf = evf_filter_load("NoRepeat", "", &err);

	if (evf == NULL) {
		printf("NoRepeat           failed: \n");
		evf_err_print(&err);
	} else
		printf("NoRepeat           success\n");

	evf_filter_free(evf);
}

void load_pressure_to_key(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "key=Key_Enter treshold=1000";

	evf = evf_filter_load("PressureToKey", params, &err);

	if (evf == NULL) {
		printf("PressureToKey      failed: \n");
		evf_err_print(&err);
	} else
		printf("PressureToKey      success\n");

	evf_filter_free(evf);
}

void load_speed_mod(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "xmod=4 ymod=4";

	evf = evf_filter_load("SpeedMod", params, &err);

	if (evf == NULL) {
		printf("SpeedMod           failed: \n");
		evf_err_print(&err);
	} else
		printf("SpeedMod           success\n");

	evf_filter_free(evf);
}

void load_barrier(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "history=10";

	evf = evf_filter_load("Barrier", params, &err);

	if (evf == NULL) {
		printf("Barrier            failed: \n");
		evf_err_print(&err);
	} else
		printf("Barrier            success\n");

	evf_filter_free(evf);
}

void load_weighted_average_abs(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "samples = 8";

	evf = evf_filter_load("WeightedAverageAbs", params, &err);

	if (evf == NULL) {
		printf("WeightedAverageAbs failed: \n");
		evf_err_print(&err);
	} else
		printf("WeightedAverageAbs success\n");

	evf_filter_free(evf);
}

void load_scale_abs(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "minx -1 miny 1 maxx 100 maxy 1000 minp 0 maxp 100";

	evf = evf_filter_load("ScaleAbs", params, &err);

	if (evf == NULL) {
		printf("ScaleAbs           failed: \n");
		evf_err_print(&err);
	} else
		printf("ScaleAbs           success\n");

	evf_filter_free(evf);
}

void load_mirror(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "MirrorAbsX = on MirrorAbsY = false MirrorRelX = Yes MirrorRelY = no";

	evf = evf_filter_load("Mirror", params, &err);

	if (evf == NULL) {
		printf("Mirror             failed: \n");
		evf_err_print(&err);
	} else
		printf("Mirror             success\n");

	evf_filter_free(evf);
}

void load_rotate(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "RotateRel no RotateAbs yes";

	evf = evf_filter_load("Rotate", params, &err);

	if (evf == NULL) {
		printf("Rotate             failed: \n");
		evf_err_print(&err);
	} else
		printf("Rotate             success\n");

	evf_filter_free(evf);
}

void load_abs2rel(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "";

	evf = evf_filter_load("Abs2Rel", params, &err);
	
	if (evf == NULL) {
		printf("Abs2Rel            failed: \n");
		evf_err_print(&err);
	} else
		printf("Abs2Rel            success\n");

	evf_filter_free(evf);
}

void load_btn2rel(void)
{
	union evf_err err;
	struct evf_filter *evf;
	char params[] = "KeyUp=Key_Up KeyDown=Key_Down KeyLeft=Key_Left KeyRight=Key_Right LeftBtn=Key_F1 RightBtn=Key_F2 MiddleBtn=Key_F3";

	evf = evf_filter_load("Btn2Rel", params, &err);
	
	if (evf == NULL) {
		printf("Btn2Rel            failed: \n");
		evf_err_print(&err);
	} else
		printf("Btn2Rel            success\n");

	evf_filter_free(evf);
}

int main(void)
{
	load_dump();
	load_no_repeat();
	load_pressure_to_key();
	load_speed_mod();
	load_barrier();
	load_weighted_average_abs();
	load_scale_abs();
	load_mirror();
	load_rotate();
	load_abs2rel();
	load_btn2rel();

	return 0;
}
