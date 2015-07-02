/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
 * Foundation, Inc.
 */

#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <rules.h>
#include <soc/msr.h>
#include <console/console.h>
#if ENV_RAMSTAGE
#include <soc/ramstage.h>
#else
#include <soc/romstage.h>
#endif
#include <stdint.h>

unsigned long tsc_freq_mhz(void)
{
	msr_t ia_core_ratios;

	ia_core_ratios = rdmsr(MSR_IACORE_RATIOS);
	return (BUS_FREQ_KHZ * ((ia_core_ratios.lo >> 16) & 0x3f)) / 1000;
}

#if !ENV_SMM

void set_max_freq(void)
{
	msr_t perf_ctl;
	msr_t msr;

	/* Enable speed step. */
	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLES, msr);

	/*
	 * Set guranteed ratio [21:16] from IACORE_RATIOS to bits [15:8] of
	 * the PERF_CTL.
	 */
	msr = rdmsr(MSR_IACORE_RATIOS);
	perf_ctl.lo = (msr.lo & 0x3f0000) >> 8;

	/*
	 * Set guranteed vid [21:16] from IACORE_VIDS to bits [7:0] of
	 * the PERF_CTL.
	 */
	msr = rdmsr(MSR_IACORE_VIDS);
	perf_ctl.lo |= (msr.lo & 0x7f0000) >> 16;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
}

#endif /* ENV_SMM */
