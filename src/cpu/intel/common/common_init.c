/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <console/console.h>
#include <cpu/x86/msr.h>
#include "common.h"

#define IA32_FEATURE_CONTROL	0x3a
#define CPUID_VMX		(1 << 5)
#define CPUID_SMX		(1 << 6)

void set_vmx(void)
{
	struct cpuid_result regs;
	msr_t msr;
	int enable = IS_ENABLED(CONFIG_ENABLE_VMX);
	int lock = IS_ENABLED(CONFIG_SET_VMX_LOCK_BIT);

	regs = cpuid(1);
	/* Check that the VMX is supported before reading or writing the MSR. */
	if (!((regs.ecx & CPUID_VMX) || (regs.ecx & CPUID_SMX))) {
		printk(BIOS_DEBUG, "CPU doesn't support VMX; exiting\n");
		return;
	}

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		printk(BIOS_ERR, "VMX is locked, so %s will do nothing\n", __func__);
		/* VMX locked. If we set it again we get an illegal
		 * instruction
		 */
		return;
	}

	/* The IA32_FEATURE_CONTROL MSR may initialize with random values.
	 * It must be cleared regardless of VMX config setting.
	 */
	msr.hi = msr.lo = 0;

	if (enable) {
		msr.lo |= (1 << 2);
		if (regs.ecx & CPUID_SMX)
			msr.lo |= (1 << 1);
	}

	wrmsr(IA32_FEATURE_CONTROL, msr);

	if (lock) {
		/* Set lock bit */
		msr.lo |= (1 << 0);
		wrmsr(IA32_FEATURE_CONTROL, msr);
	}

	printk(BIOS_DEBUG, "VMX status: %s, %s\n", enable ? "enabled" : "disabled",
		lock ? "locked" : "unlocked");
}
