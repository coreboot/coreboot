/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
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

#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <soc/cpu.h>
#include <console/console.h>
#include <delay.h>
#include "chip.h"

void set_max_freq(void)
{
	msr_t msr, msr_rd;
	unsigned int eax;

	eax = cpuid_eax(CPUID_LEAF_PM);

	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	eax &= 0x2;
	if ((!eax) && ((msr.hi & APL_BURST_MODE_DISABLE) == 0)) {
		/* Burst Mode has been factory configured as disabled
		 * and is not available in this physical processor
		 * package.
		 */
		printk(BIOS_DEBUG, "Burst Mode is factory disabled\n");
		return;
	}

	/* Enable burst mode */
	msr.hi &= ~APL_BURST_MODE_DISABLE;
	wrmsr(MSR_IA32_MISC_ENABLES, msr);

	/* Enable speed step. */
	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	msr.lo |= 1 << 16;
	wrmsr(MSR_IA32_MISC_ENABLES, msr);

	/* Set P-State ratio */
	msr = rdmsr(IA32_PERF_CTL);
	msr.lo &= ~0xff00;

	/* Read the frequency limit ratio and set it properly in PERF_CTL */
	msr_rd = rdmsr(FREQ_LIMIT_RATIO);
	msr.lo |= (msr_rd.lo & 0xff) << 8;

	wrmsr(IA32_PERF_CTL, msr);
}
