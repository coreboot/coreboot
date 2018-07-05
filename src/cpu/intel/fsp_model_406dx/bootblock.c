/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering LLC.
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

#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/microcode/microcode.c>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <arch/io.h>
#include <reset.h>
#include <southbridge/intel/fsp_rangeley/soc.h>

#include "model_406dx.h"

/*
 * check for a warm reset and do a hard reset instead.
 */
static void check_for_warm_reset(void)
{

	/*
	 * Check if INIT# is asserted by port 0xCF9 and whether RCBA has been
	 * set.   If either is true, then this is a warm reset so execute a
	 * Hard Reset
	 */
	if ((inb(0xcf9) == 0x04) ||
			(pci_io_read_config32(SOC_LPC_DEV, RCBA)
			& RCBA_ENABLE)) {
		outb(0x00, 0xcf9);
		outb(0x06, 0xcf9);
	}
}

static void set_var_mtrr(int reg, uint32_t base, uint32_t size, int type)
{
	/* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
	msr_t basem, maskm;
	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRR_PHYS_BASE(reg), basem);
	maskm.lo = ~(size - 1) | MTRR_PHYS_MASK_VALID;
	maskm.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRR_PHYS_MASK(reg), maskm);
}

static void enable_rom_caching(void)
{
	msr_t msr;

	disable_cache();
	set_var_mtrr(1, CACHE_ROM_BASE, CACHE_ROM_SIZE, MTRR_TYPE_WRPROT);
	enable_cache();

	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRR_DEF_TYPE_MSR, msr);
}

static void set_no_evict_mode_msr(void)
{
	msr_t msr;
	msr.hi = 0x00000000;
	msr.lo = 0x00000000;

	wrmsr(MSR_NO_EVICT_MODE, msr);
}

static void bootblock_cpu_init(void)
{
	/* Check for Warm Reset */
	check_for_warm_reset();

	/* Load microcode before any caching. */
	intel_update_microcode_from_cbfs();

	enable_rom_caching();
	set_no_evict_mode_msr();
}
