/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Philipp Hug <philipp@hug.cx>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <arch/encoding.h>
#include <console/console.h>
#include <stddef.h>
#include <timer.h>
#include <mcall.h>

void timer_monotonic_get(struct mono_time *mt)
{
	if (HLS()->time == NULL)
		die("time not set in HLS");
	mono_time_set_usecs(mt, (long)read64((void *)(HLS()->time)));
}
