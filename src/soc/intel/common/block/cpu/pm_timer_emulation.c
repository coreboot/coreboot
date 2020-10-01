/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <soc/iomap.h>
#include <soc/pm.h>

void enable_pm_timer_emulation(void)
{
	msr_t msr;

	if (!CONFIG_CPU_XTAL_HZ)
		return;

	/*
	 * The derived frequency is calculated as follows:
	 * (clock * msr[63:32]) >> 32 = target frequency.
	 * Back solve the multiplier so the 3.579545MHz ACPI timer frequency is used.
	 */
	msr.hi = (3579545ULL << 32) / CONFIG_CPU_XTAL_HZ;
	/* Set PM1 timer IO port and enable */
	msr.lo = (EMULATE_DELAY_VALUE << EMULATE_DELAY_OFFSET_VALUE) |
		  EMULATE_PM_TMR_EN | (ACPI_BASE_ADDRESS + PM1_TMR);
	wrmsr(MSR_EMULATE_PM_TIMER, msr);
}
