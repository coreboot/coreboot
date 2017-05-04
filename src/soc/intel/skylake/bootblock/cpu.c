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
 */

#include <cpu/x86/msr.h>
#include <delay.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/msr.h>
#include <reset.h>
#include <soc/bootblock.h>

/* Soft Reset Data Register Bit 12 = MAX Boot Frequency */
#define SPI_STRAP_MAX_FREQ	(1<<12)
/* Soft Reset Data Register Bit 6-11 = Flex Ratio */
#define FLEX_RATIO_BIT	6

static void set_pch_cpu_strap(u8 flex_ratio)
{
	u32 soft_reset_data;

	/* Soft Reset Data Register Bit 12 = MAX Boot Frequency
	 * Bit 6-11 = Flex Ratio
	 * Soft Reset Data register located at SPIBAR0 offset 0xF8[0:15].
	 */
	soft_reset_data = SPI_STRAP_MAX_FREQ;
	soft_reset_data |= (flex_ratio << FLEX_RATIO_BIT);
	fast_spi_set_strap_msg_data(soft_reset_data);
}

static void set_flex_ratio_to_tdp_nominal(void)
{
	msr_t flex_ratio, msr;
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

	/* Set PCH Soft Reset Data Register with new Flex Ratio */
	set_pch_cpu_strap(nominal_ratio);

	/* Delay before reset to avoid potential TPM lockout */
	mdelay(30);

	/* Issue soft reset, will be "CPU only" due to soft reset data */
	soft_reset();
}

void bootblock_cpu_init(void)
{
	fast_spi_cache_bios_region();
	/* Set flex ratio and reset if needed */
	set_flex_ratio_to_tdp_nominal();
}

void set_max_freq(void)
{
	if (cpu_config_tdp_levels())
		/* Set to nominal TDP ratio */
		cpu_set_p_state_to_nominal_tdp_ratio();
	else
		/* Set to max non Turbo ratio */
		cpu_set_p_state_to_max_non_turbo_ratio();
}
