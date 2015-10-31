/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <soc/iosf.h>
#include <cpu/intel/microcode/microcode.c>

static void set_var_mtrr(int reg, uint32_t base, uint32_t size, int type)
{
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
	/* Why only top 4MiB ? */
	set_var_mtrr(1, 0xffc00000, 4*1024*1024, MTRR_TYPE_WRPROT);
	enable_cache();

	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRR_DEF_TYPE_MSR, msr);
}

static void setup_mmconfig(void)
{
	uint32_t reg;

	/*
	 * Set up the MMCONF range. The register lives in the BUNIT. The
	 * IO variant of the config access needs to be used initially to
	 * properly configure as the IOSF access registers live in PCI
	 * config space.
	 */
	reg = 0;
	/* Clear the extended register. */
	pci_io_write_config32(IOSF_PCI_DEV, MCRX_REG, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | 1;
	pci_io_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) | IOSF_PORT(IOSF_PORT_BUNIT) |
	      IOSF_REG(BUNIT_MMCONF_REG) | IOSF_BYTE_EN;
	pci_io_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}

static void bootblock_cpu_init(void)
{
	/* Allow memory-mapped PCI config access. */
	setup_mmconfig();

	/* Load microcode before any caching. */
	intel_update_microcode_from_cbfs();
	enable_rom_caching();
}
