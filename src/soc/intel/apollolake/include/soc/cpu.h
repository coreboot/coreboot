/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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
 */

#ifndef _SOC_APOLLOLAKE_CPU_H_
#define _SOC_APOLLOLAKE_CPU_H_

#include <cpu/x86/msr.h>
#include <intelblocks/msr.h>

/* Common Timer Copy (CTC) frequency - 19.2MHz. */
#define CTC_FREQ		19200000

struct device;
void apollolake_init_cpus(struct device *dev);
void mainboard_devtree_update(struct device *dev);

/* Flush L1D to L2 */
static inline void flush_l1d_to_l2(void)
{
	msr_t msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= FLUSH_DL1_L2;
	wrmsr(MSR_POWER_MISC, msr);
}
#endif /* _SOC_APOLLOLAKE_CPU_H_ */
