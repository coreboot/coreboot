/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/cpu.h>
#include <stdlib.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/romstage.h>

u32 cpu_family_model(void)
{
	return cpuid_eax(1) & 0x0fff0ff0;
}

void set_max_freq(void)
{
	msr_t msr, perf_ctl, platform_info;

	/* Check for configurable TDP option */
	platform_info = rdmsr(MSR_PLATFORM_INFO);

	if ((platform_info.hi >> 1) & 3) {
		/* Set to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info bits 15:8 give max ratio */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}

	perf_ctl.hi = 0;
	wrmsr(IA32_PERF_CTL, perf_ctl);

	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
		((perf_ctl.lo >> 8) & 0xff) * CPU_BCLK);
}
