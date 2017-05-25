/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <intelblocks/msr.h>

unsigned long tsc_freq_mhz(void)
{
	msr_t msr = rdmsr(MSR_PLATFORM_INFO);
	return (CONFIG_CPU_BCLK_MHZ * ((msr.lo >> 8) & 0xff));
}
