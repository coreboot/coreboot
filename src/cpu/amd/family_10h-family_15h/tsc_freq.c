/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Raptor Engineering
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

unsigned long tsc_freq_mhz(void)
{
	msr_t msr;
	uint8_t cpufid;
	uint8_t cpudid;

	/* On Family 10h/15h CPUs the TSC increments
         * at the P0 clock rate.  Read the P0 clock
         * frequency from the P0 MSR and convert
         * to MHz.  See also the Family 15h BKDG
         * Rev. 3.14 page 569.
         */
	msr = rdmsr(0xc0010064);
	cpufid = (msr.lo & 0x3f);
	cpudid = (msr.lo & 0x1c0) >> 6;

	return (100 * (cpufid + 0x10)) / (0x01 << cpudid);
}
