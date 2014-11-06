/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <timer.h>
#include <timestamp.h>
#include <arch/cpu.h>
#include <soc/cpu.h>

static int get_count_mhz_freq(void)
{
	static unsigned count_mhz_freq;

	if (!count_mhz_freq) {
		if (IMG_PLATFORM_ID() != IMG_PLATFORM_ID_SILICON)
			count_mhz_freq = 25; /* FPGA board */
			/*
			 * Will need some means of finding out the counter
			 * frequency on a real SOC
			 */
	}
	return count_mhz_freq;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mt->microseconds = (long)timestamp_get();
}

uint64_t timestamp_get(void)
{
	return read_c0_count()/get_count_mhz_freq();
}
