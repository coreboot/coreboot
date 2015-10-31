/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <libpayload.h>
#include <arch/cpu.h>

uint64_t timer_hz(void)
{
	return lib_sysinfo.cpu_khz * 1000;
}

uint64_t timer_raw_value(void)
{
	static uint64_t total_ticks = 0;
	uint8_t overflow = 0;
	uint32_t current_ticks = read_c0_count() * 2;

	/* It assumes only one overflow happened since the last call */
	if (current_ticks <= (uint32_t)total_ticks)
		overflow = 1;
	/* The least significant part(32 bits) of total_ticks will always
	 * become equal to current ticks */
	total_ticks = (((total_ticks  >> 32) + overflow) << 32) +
				current_ticks;
	return total_ticks;
}
