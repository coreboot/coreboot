/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors. All rights reserved.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <arch/cpu.h>

static int turbo_state = TURBO_UNKNOWN;

static const char *turbo_state_desc[] = {
	[TURBO_UNKNOWN]		= "unknown",
	[TURBO_UNAVAILABLE]	= "unavailable",
	[TURBO_DISABLED]	= "available but hidden",
	[TURBO_ENABLED]		= "available and visible"
};

/*
 * Determine the current state of Turbo and cache it for later.
 * Turbo is a package level config so it does not need to be
 * enabled on every core.
 */
int get_turbo_state(void)
{
	struct cpuid_result cpuid_regs;
	int turbo_en, turbo_cap;
	msr_t msr;

	/* Return cached state if available */
	if (turbo_state != TURBO_UNKNOWN)
		return turbo_state;

	cpuid_regs = cpuid(CPUID_LEAF_PM);
	turbo_cap = !!(cpuid_regs.eax & PM_CAP_TURBO_MODE);

	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	turbo_en = !(msr.hi & H_MISC_DISABLE_TURBO);

	if (!turbo_cap && turbo_en) {
		/* Unavailable */
		turbo_state = TURBO_UNAVAILABLE;
	} else if (!turbo_cap && !turbo_en) {
		/* Available but disabled */
		turbo_state = TURBO_DISABLED;
	} else if (turbo_cap && turbo_en) {
		/* Available */
		turbo_state = TURBO_ENABLED;
	}

	printk(BIOS_INFO, "Turbo is %s\n", turbo_state_desc[turbo_state]);
	return turbo_state;
}

/*
 * Try to enable Turbo mode.
 */
void enable_turbo(void)
{
	msr_t msr;

	/* Only possible if turbo is available but hidden */
	if (get_turbo_state() == TURBO_DISABLED) {
		/* Clear Turbo Disable bit in Misc Enables */
		msr = rdmsr(MSR_IA32_MISC_ENABLES);
		msr.hi &= ~H_MISC_DISABLE_TURBO;
		wrmsr(MSR_IA32_MISC_ENABLES, msr);

		/* Update cached turbo state */
		turbo_state = TURBO_ENABLED;
		printk(BIOS_INFO, "Turbo has been enabled\n");
	}
}
