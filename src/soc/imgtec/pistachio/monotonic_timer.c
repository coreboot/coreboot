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
 */

#include <arch/cpu.h>
#include <soc/cpu.h>
#include <stdint.h>
#include <timer.h>
#include <timestamp.h>

#define PISTACHIO_CLOCK_SWITCH		0xB8144200
#define MIPS_EXTERN_PLL_BYPASS_MASK	0x00000002

static int get_count_mhz_freq(void)
{
	static unsigned count_mhz_freq;

	if (!count_mhz_freq) {
		if (IMG_PLATFORM_ID() != IMG_PLATFORM_ID_SILICON)
			count_mhz_freq = 25; /* FPGA board */
		else {
			/* If MIPS PLL external bypass bit is set, it means
			 * that the MIPS PLL is already set up to work at a
			 * frequency of 550 MHz; otherwise, the crystal is
			 * used with a frequency of 52 MHz
			 */
			if (read32(PISTACHIO_CLOCK_SWITCH) &
				MIPS_EXTERN_PLL_BYPASS_MASK)
				/* Half MIPS PLL freq. */
				count_mhz_freq = 275;
			else
				/* Half Xtal freq. */
				count_mhz_freq = 26;
		}
	}
	return count_mhz_freq;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, read_c0_count() / get_count_mhz_freq());
}
