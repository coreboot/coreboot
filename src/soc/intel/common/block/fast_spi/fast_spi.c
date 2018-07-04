/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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
#include <assert.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fast_spi_def.h>
#include <intelblocks/fast_spi.h>
#include <lib.h>
#include <soc/pci_devs.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdlib.h>
#include <string.h>

/*
 * Get the FAST_SPIBAR.
 */
void *fast_spi_get_bar(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_SPI;
#else
	struct device *dev = PCH_DEV_SPI;
#endif
	uintptr_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	assert(bar != 0);
	/*
	 * Bits 31-12 are the base address as per EDS for SPI,
	 * Don't care about 0-11 bit
	 */
	return (void *)(bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

/*
 * Disable the BIOS write protect and Enable Prefetching and Caching.
 */
void fast_spi_init(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_SPI;
#else
	struct device *dev = PCH_DEV_SPI;
#endif
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, SPIBAR_BIOS_CONTROL);

	/* Disable the BIOS write protect so write commands are allowed. */
	bios_cntl &= ~SPIBAR_BIOS_CONTROL_EISS;
	bios_cntl |= SPIBAR_BIOS_CONTROL_WPD;
	/* Enable Prefetching and caching. */
	bios_cntl |= SPIBAR_BIOS_CONTROL_PREFETCH_ENABLE;
	bios_cntl &= ~SPIBAR_BIOS_CONTROL_CACHE_DISABLE;

	pci_write_config8(dev, SPIBAR_BIOS_CONTROL, bios_cntl);
}

/*
 * Set FAST_SPIBAR BIOS Control register based on input bit field.
 */
static void fast_spi_set_bios_control_reg(uint8_t bios_cntl_bit)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_SPI;
#else
	struct device *dev = PCH_DEV_SPI;
#endif
	uint8_t bc_cntl;

	assert((bios_cntl_bit & (bios_cntl_bit - 1)) == 0);
	bc_cntl = pci_read_config8(dev, SPIBAR_BIOS_CONTROL);
	bc_cntl |= bios_cntl_bit;
	pci_write_config8(dev, SPIBAR_BIOS_CONTROL, bc_cntl);
}

/*
 * Ensure an additional read back after performing lock down
 */
static void fast_spi_read_post_write(uint8_t reg)
{
	pci_read_config8(PCH_DEV_SPI, reg);
}

/*
 * Set FAST_SPIBAR BIOS Control BILD bit.
 */
void fast_spi_set_bios_interface_lock_down(void)
{
	fast_spi_set_bios_control_reg(SPIBAR_BIOS_CONTROL_BILD);

	fast_spi_read_post_write(SPIBAR_BIOS_CONTROL);
}

/*
 * Set FAST_SPIBAR BIOS Control LE bit.
 */
void fast_spi_set_lock_enable(void)
{
	fast_spi_set_bios_control_reg(SPIBAR_BIOS_CONTROL_LOCK_ENABLE);


	fast_spi_read_post_write(SPIBAR_BIOS_CONTROL);
}

/*
 * Set FAST_SPIBAR BIOS Control EISS bit.
 */
void fast_spi_set_eiss(void)
{
	fast_spi_set_bios_control_reg(SPIBAR_BIOS_CONTROL_EISS);

	fast_spi_read_post_write(SPIBAR_BIOS_CONTROL);
}

/*
 * Set FAST_SPI opcode menu.
 */
void fast_spi_set_opcode_menu(void)
{
	void *spibar = fast_spi_get_bar();

	write16(spibar + SPIBAR_PREOP, SPI_OPPREFIX);
	write16(spibar + SPIBAR_OPTYPE, SPI_OPTYPE);
	write32(spibar + SPIBAR_OPMENU_LOWER, SPI_OPMENU_LOWER);
	write32(spibar + SPIBAR_OPMENU_UPPER, SPI_OPMENU_UPPER);
}

/*
 * Lock FAST_SPIBAR.
 * Use 16bit write to avoid touching two upper bytes what may cause the write
 * cycle to fail in case a prior transaction has not completed.
 * While WRSDIS is lockable with FLOCKDN, writing both in the same
 * cycle is guaranteed to work by design.
 *
 * Avoid read->modify->write not to clear RW1C bits unintentionally.
 */
void fast_spi_lock_bar(void)
{
	void *spibar = fast_spi_get_bar();
	uint16_t hsfs = SPIBAR_HSFSTS_FLOCKDN;

	if (IS_ENABLED(CONFIG_FAST_SPI_DISABLE_WRITE_STATUS))
		hsfs |= SPIBAR_HSFSTS_WRSDIS;

	write16(spibar + SPIBAR_HSFSTS_CTL, hsfs);
}

/*
 * Set FAST_SPIBAR + DLOCK (0x0C) register bits to discrete lock the
 * FAST_SPI Protected Range (PR) registers.
 */
void fast_spi_pr_dlock(void)
{
	void *spibar = fast_spi_get_bar();
	uint32_t dlock;

	dlock = read32(spibar + SPIBAR_DLOCK);
	dlock |= (SPIBAR_DLOCK_PR0LOCKDN | SPIBAR_DLOCK_PR1LOCKDN
			| SPIBAR_DLOCK_PR2LOCKDN | SPIBAR_DLOCK_PR3LOCKDN
			| SPIBAR_DLOCK_PR4LOCKDN);

	write32(spibar + SPIBAR_DLOCK, dlock);
}

/*
 * Set FAST_SPIBAR Soft Reset Data Register value.
 */
void fast_spi_set_strap_msg_data(uint32_t soft_reset_data)
{
	void *spibar = fast_spi_get_bar();
	uint32_t ssl, ssms;

	/* Set Strap Lock Disable */
	ssl = read32(spibar + SPIBAR_RESET_LOCK);
	ssl &= ~SPIBAR_RESET_LOCK_ENABLE;
	write32(spibar + SPIBAR_RESET_LOCK, ssl);

	/* Write Soft Reset Data register at SPIBAR0 offset 0xF8[0:15] */
	write32(spibar + SPIBAR_RESET_DATA, soft_reset_data);

	/* Set Strap Mux Select  set to '1' */
	ssms = read32(spibar + SPIBAR_RESET_CTRL);
	ssms |= SPIBAR_RESET_CTRL_SSMC;
	write32(spibar + SPIBAR_RESET_CTRL, ssms);

	/* Set Strap Lock Enable */
	ssl = read32(spibar + SPIBAR_RESET_LOCK);
	ssl |= SPIBAR_RESET_LOCK_ENABLE;
	write32(spibar + SPIBAR_RESET_LOCK, ssl);
}

/*
 * Returns bios_start and fills in size of the BIOS region.
 */
size_t fast_spi_get_bios_region(size_t *bios_size)
{
	size_t bios_start, bios_end;
	/*
	 * BIOS_BFPREG provides info about BIOS Flash Primary Region
	 * Base and Limit.
	 * Base and Limit fields are in units of 4KiB.
	 */
	uint32_t val = read32(fast_spi_get_bar() + SPIBAR_BFPREG);

	bios_start = (val & SPIBAR_BFPREG_PRB_MASK) * 4 * KiB;
	bios_end = (((val & SPIBAR_BFPREG_PRL_MASK) >>
		     SPIBAR_BFPREG_PRL_SHIFT) + 1) * 4 * KiB;
	*bios_size = bios_end - bios_start;
	return bios_start;
}

void fast_spi_cache_bios_region(void)
{
	size_t bios_size;
	uint32_t alignment;
	const int type = MTRR_TYPE_WRPROT;
	uintptr_t base;

	/* Only the IFD BIOS region is memory mapped (at top of 4G) */
	fast_spi_get_bios_region(&bios_size);

	if (!bios_size)
		return;

	/* Round to power of two */
	alignment = 1UL << (log2_ceil(bios_size));
	bios_size = ALIGN_UP(bios_size, alignment);
	base = 4ULL*GiB - bios_size;

	if (ENV_RAMSTAGE) {
		mtrr_use_temp_range(base, bios_size, type);
	} else {
		int mtrr = get_free_var_mtrr();

		if (mtrr == -1)
			return;

		set_var_mtrr(mtrr, base, bios_size, type);
	}
}

/*
 * Program temporary BAR for SPI in case any of the stages before ramstage need
 * to access FAST_SPI MMIO regs. Ramstage will assign a new BAR during PCI
 * enumeration.
 */
void fast_spi_early_init(uintptr_t spi_base_address)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_SPI;
#else
	struct device *dev = PCH_DEV_SPI;
#endif
	uint8_t pcireg;

	/* Assign Resources to SPI Controller */
	/* Clear BIT 1-2 SPI Command Register */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for SPI */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0,
		spi_base_address | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/* Enable Bus Master and MMIO Space */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	/* Initialize SPI to allow BIOS to write/erase on flash. */
	fast_spi_init();
}

/* Read SPI Write Protect disable status. */
bool fast_spi_wpd_status(void)
{
	return pci_read_config16(PCH_DEV_SPI, SPIBAR_BIOS_CONTROL) &
		SPIBAR_BIOS_CONTROL_WPD;
}

/* Enable SPI Write Protect. */
void fast_spi_enable_wp(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_SPI;
#else
	struct device *dev = PCH_DEV_SPI;
#endif
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, SPIBAR_BIOS_CONTROL);
	bios_cntl &= ~SPIBAR_BIOS_CONTROL_WPD;
	pci_write_config8(dev, SPIBAR_BIOS_CONTROL, bios_cntl);
}
