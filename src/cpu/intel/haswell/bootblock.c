/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/bootblock.h>
#include <cpu/x86/msr.h>
#include <arch/io.h>
#include <delay.h>
#include <halt.h>

#include "haswell.h"

#include <southbridge/intel/lynxpoint/pch.h>

static void set_flex_ratio_to_tdp_nominal(void)
{
	msr_t flex_ratio, msr;
	u32 soft_reset;
	u8 nominal_ratio;

	/* Check for Flex Ratio support */
	flex_ratio = rdmsr(MSR_FLEX_RATIO);
	if (!(flex_ratio.lo & FLEX_RATIO_EN))
		return;

	/* Check for >0 configurable TDPs */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if (((msr.hi >> 1) & 3) == 0)
		return;

	/* Use nominal TDP ratio for flex ratio */
	msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
	nominal_ratio = msr.lo & 0xff;

	/* See if flex ratio is already set to nominal TDP ratio */
	if (((flex_ratio.lo >> 8) & 0xff) == nominal_ratio)
		return;

	/* Set flex ratio to nominal TDP ratio */
	flex_ratio.lo &= ~0xff00;
	flex_ratio.lo |= nominal_ratio << 8;
	flex_ratio.lo |= FLEX_RATIO_LOCK;
	wrmsr(MSR_FLEX_RATIO, flex_ratio);

	/* Set flex ratio in soft reset data register bits 11:6.
	 * RCBA region is enabled in southbridge bootblock */
	soft_reset = RCBA32(SOFT_RESET_DATA);
	soft_reset &= ~(0x3f << 6);
	soft_reset |= (nominal_ratio & 0x3f) << 6;
	RCBA32(SOFT_RESET_DATA) = soft_reset;

	/* Set soft reset control to use register value */
	RCBA32_OR(SOFT_RESET_CTRL, 1);

	/* Delay before reset to avoid potential TPM lockout */
	if (CONFIG(TPM1) || CONFIG(TPM2))
		mdelay(30);

	/* Issue warm reset, will be "CPU only" due to soft reset data */
	outb(0x0, 0xcf9);
	outb(0x6, 0xcf9);
	halt();
}

void bootblock_early_cpu_init(void)
{
	/* Set flex ratio and reset if needed */
	set_flex_ratio_to_tdp_nominal();
}
