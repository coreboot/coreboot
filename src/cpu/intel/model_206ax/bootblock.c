/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <arch/io.h>
#include <arch/x86/include/cf9_reset.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <option.h>
#include <stdint.h>

#include "model_206ax.h"

#if CONFIG(SOUTHBRIDGE_INTEL_BD82X6X) || CONFIG(SOUTHBRIDGE_INTEL_C216)
/* Needed for RCBA access to set Soft Reset Data register */
#include <southbridge/intel/bd82x6x/pch.h>
#else
#error "CPU must be paired with Intel BD82X6X or C216 southbridge"
#endif

static u8 set_flex_ratio_to_tdp_nominal(void)
{
	msr_t flex_ratio, msr;
	u8 nominal_ratio;

	/* Minimum CPU revision for configurable TDP support */
	if (cpuid_eax(1) < IVB_CONFIG_TDP_MIN_CPUID)
		return 0;

	/* Check for Flex Ratio support */
	flex_ratio = rdmsr(MSR_FLEX_RATIO);
	if (!(flex_ratio.lo & FLEX_RATIO_EN))
		return 0;

	/* Check for >0 configurable TDPs */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if (((msr.hi >> 1) & 3) == 0)
		return 0;

	/* Use nominal TDP ratio for flex ratio */
	msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
	nominal_ratio = msr.lo & 0xff;

	/* See if flex ratio is already set to nominal TDP ratio */
	if (((flex_ratio.lo >> 8) & 0xff) != nominal_ratio) {
		/* Set flex ratio to nominal TDP ratio */
		flex_ratio.lo &= ~0xff00;
		flex_ratio.lo |= nominal_ratio << 8;
		flex_ratio.lo |= FLEX_RATIO_LOCK;
		wrmsr(MSR_FLEX_RATIO, flex_ratio);
	}

	return nominal_ratio;
}

static void set_soft_reset(u8 nominal_ratio, bool smt_opt)
{
	/* Set flex ratio in soft reset data register bits 11:6, set !smt_cmos
	 * in bit 0 to disable smt. RCBA region is enabled in southbridge
	 * bootblock */
	u32 soft_reset = RCBA32(SOFT_RESET_DATA);

	if (nominal_ratio) {
		soft_reset &= ~(0x3f << 6);
		soft_reset |= (nominal_ratio & 0x3f) << 6;
	}

	if (intel_ht_supported()) {
		soft_reset &= ~1;
		soft_reset |= !smt_opt;
		printk(BIOS_DEBUG, "CPU supports SMT, respecting option hyper_threading=%s\n",
		       smt_opt ? "Enable" : "Disable");
	}

	/* The value of soft reset data register can survive across warm reset. */
	if (RCBA32(SOFT_RESET_DATA) == soft_reset) {
		/* it already has needed value, lock it and continue booting. */
		RCBA32_OR(SOFT_RESET_LOCK, 1);
		return;
	}

	RCBA32(SOFT_RESET_DATA) = soft_reset;

	/* Set soft reset control to use register value */
	RCBA32_OR(SOFT_RESET_CTRL, 1);

	/* Issue warm reset, will be "CPU only" due to soft reset data */
	system_reset();
}

void bootblock_cpu_init(void)
{
	/* Set flex ratio and smt, and reset if needed */
	set_soft_reset(set_flex_ratio_to_tdp_nominal(), get_uint_option("hyper_threading", 1));
}
