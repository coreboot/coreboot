/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include "model_206ax.h"

/* MSR Documentation based on
 * "Sandy Bridge Processor Family BIOS Writer's Guide (BWG)"
 * Document Number 504790
 * Revision 1.6.0, June 2012 */

static void msr_set_bit(unsigned int reg, unsigned int bit)
{
	msr_t msr = rdmsr(reg);

	if (bit < 32) {
		if (msr.lo & (1 << bit))
			return;
		msr.lo |= 1 << bit;
	} else {
		if (msr.hi & (1 << (bit - 32)))
			return;
		msr.hi |= 1 << (bit - 32);
	}

	wrmsr(reg, msr);
}

void intel_model_206ax_finalize_smm(void)
{
	/* Lock C-State MSR */
	msr_set_bit(MSR_PMG_CST_CONFIG_CONTROL, 15);

	/* Lock AES-NI only if supported */
	if (cpuid_ecx(1) & (1 << 25))
		msr_set_bit(MSR_FEATURE_CONFIG, 0);

#ifdef LOCK_POWER_CONTROL_REGISTERS
	/*
	 * Lock the power control registers.
	 *
	 * These registers can be left unlocked if modifying power
	 * limits from the OS is desirable. Modifying power limits
	 * from the OS can be especially useful for experimentation
	 * during  early phases of system bringup while the thermal
	 * power envelope is being proven.
	 */

	msr_set_bit(MSR_PP0_CURRENT_CONFIG, 31);
	msr_set_bit(MSR_PP1_CURRENT_CONFIG, 31);
	msr_set_bit(MSR_PKG_POWER_LIMIT, 63);
	msr_set_bit(MSR_PP0_POWER_LIMIT, 31);
	msr_set_bit(MSR_PP1_POWER_LIMIT, 31);
#endif

	/* Lock TM interrupts - route thermal events to all processors */
	msr_set_bit(MSR_MISC_PWR_MGMT, 22);

	/* Lock memory configuration to protect SMM */
	msr_set_bit(MSR_LT_LOCK_MEMORY, 0);
}
