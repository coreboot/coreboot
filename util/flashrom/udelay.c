/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <sys/time.h>
#include <stdio.h>
#include "flash.h"

// count to a billion. Time it. If it's < 1 sec, count to 10B, etc.
unsigned long micro = 1;

void myusec_delay(int time)
{
	volatile unsigned long i;
	for (i = 0; i < time * micro; i++) ;
}

void myusec_calibrate_delay()
{
	int count = 1000;
	unsigned long timeusec;
	struct timeval start, end;
	int ok = 0;

	printf("Calibrating delay loop... ");

	while (!ok) {
		gettimeofday(&start, 0);
		myusec_delay(count);
		gettimeofday(&end, 0);
		timeusec = 1000000 * (end.tv_sec - start.tv_sec) +
		    (end.tv_usec - start.tv_usec);
		count *= 2;
		if (timeusec < 1000000 / 4)
			continue;
		ok = 1;
	}

	// compute one microsecond. That will be count / time
	micro = count / timeusec;

	printf_debug("%ldM loops per second. ", (unsigned long)micro);
	printf("OK\n");
}
