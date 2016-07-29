/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>

static unsigned long bus_freq_khz(void)
{
	/* CPU freq = 400 MHz */
	return 400 * 1000;
}

unsigned long tsc_freq_mhz(void)
{
	/* assume ratio=1 */
	unsigned bclk_khz = bus_freq_khz();

	if (!bclk_khz)
		return 0;

	return (bclk_khz * 1) / 1000;
}
