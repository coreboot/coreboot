/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#include <arch/io.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode/microcode.c>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/gpio.h>
#include <reset.h>

/*
 * check for a warm reset and do a hard reset instead.
 */
static void check_for_warm_reset(void)
{

	/*
	 * Check if INIT# is asserted by port 0xCF9 and whether RCBA has been set.
	 * If either is true, then this is a warm reset so execute a Hard Reset
	 */
	if ( (inb(0xcf9) == 0x04) ||
			(pci_io_read_config32(LPC_BDF, RCBA) & RCBA_ENABLE) ) {
		outb(0x00, 0xcf9);
		outb(0x06, 0xcf9);
	}
}

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

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u32 *bcr = (u32 *)(SPI_BASE_ADDRESS + BCR);
	/* Enable caching and prefetching in the SPI controller. */
	write32(bcr, (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH);
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

static void setup_mmconfig(void)
{
	uint32_t reg;

	/* Set up the MMCONF range. The register lives in the BUNIT. The
	 * IO variant of the config access needs to be used initially to
	 * properly configure as the IOSF access registers live in PCI
	 * config space. */
	reg = 0;
	/* Clear the extended register. */
	pci_io_write_config32(IOSF_PCI_DEV, MCRX_REG, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | 1;
	pci_io_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) | IOSF_PORT(IOSF_PORT_BUNIT) |
	      IOSF_REG(BUNIT_MMCONF_REG) | IOSF_BYTE_EN;
	pci_io_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}

static const uint8_t lpc_pads[12] = {
	70, 68, 67, 66, 69, 71, 65, 72, 86, 90, 88, 92,
};

static void set_up_lpc_pads(void)
{
	uint32_t reg = IO_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, IOBASE, reg);

	for (reg = 0; reg < 12; reg++)
		score_select_func(lpc_pads[reg], 1);
}

static void bootblock_cpu_init(void)
{
	check_for_warm_reset();

	/* Load microcode before any caching. */
	intel_update_microcode_from_cbfs();

	/* Allow memory-mapped PCI config access. */
	setup_mmconfig();
	enable_rom_caching();
	enable_spi_prefetch();
	set_up_lpc_pads();
}
