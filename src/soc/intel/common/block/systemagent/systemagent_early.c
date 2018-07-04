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
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include "systemagent_def.h"
#include <timer.h>

#if !ENV_RAMSTAGE
void bootblock_systemagent_early_init(void)
{
	uint32_t reg;
	uint8_t pciexbar_length;

	/*
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	reg = 0;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR + 4, reg);

	/* Get PCI Express Region Length */
	switch (CONFIG_SA_PCIEX_LENGTH) {
	case 256 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_256MB;
		break;
	case 128 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_128MB;
		break;
	case 64 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_64MB;
		break;
	default:
		pciexbar_length = PCIEXBAR_LENGTH_256MB;
	}
	reg = CONFIG_MMCONF_BASE_ADDRESS | (pciexbar_length << 1)
				| PCIEXBAR_PCIEXBAREN;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR, reg);

	/*
	 * TSEG defines the base of SMM range. BIOS determines the base
	 * of TSEG memory which must be at or below Graphics base of GTT
	 * Stolen memory, hence its better to clear TSEG register early
	 * to avoid power on default non-zero value (if any).
	 */
	pci_write_config32(SA_DEV_ROOT, TSEG, 0);
}
#endif

void sa_set_pci_bar(const struct sa_mmio_descriptor *fixed_set_resources,
		size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		uintptr_t base;
		unsigned int index;

		index = fixed_set_resources[i].index;
		/* Check if PCI BAR already enabled */
		base = pci_read_config32(SA_DEV_ROOT, index);

		/* If enabled don't program it. */
		if (base & 0x1)
			return;

		base = fixed_set_resources[i].base;

		pci_write_config32(SA_DEV_ROOT, index, base | 1);
	}
}

/*
 * There are special BARs that actually are programmed in the MCHBAR. These
 * Intel special features, but they do consume resources that need to be
 * accounted for.
 */
void sa_set_mch_bar(const struct sa_mmio_descriptor *fixed_set_resources,
		size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		uintptr_t base;
		unsigned int index;

		base = fixed_set_resources[i].base;
		index = fixed_set_resources[i].index;
		write32((void *)(MCH_BASE_ADDRESS + index), base | 1);
	}
}

void enable_pam_region(void)
{
	/* All read and writes in this region are serviced by DRAM */
	pci_write_config8(SA_DEV_ROOT, PAM0, 0x30);
	pci_write_config8(SA_DEV_ROOT, PAM1, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM2, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM3, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM4, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM5, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM6, 0x33);
}

void enable_bios_reset_cpl(void)
{
	u8 bios_reset_cpl;

	/*
	 * Set bits 0+1 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	bios_reset_cpl = MCHBAR8(BIOS_RESET_CPL);
	bios_reset_cpl |= 3;
	MCHBAR8(BIOS_RESET_CPL) = bios_reset_cpl;
}

uintptr_t sa_get_tolud_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, TOLUD), 1*MiB);
}

static uint16_t sa_get_ggc_reg(void)
{
	return pci_read_config16(SA_DEV_ROOT, GGC);
}

/*
 * Internal Graphics Pre-allocated Memory - As per Intel FSP UPD Header
 * definition, size of memory preallocatred for internal graphics can be
 * configured based on below lists:
 *
 * 0x00:0MB, 0x01:32MB, 0x02:64MB, 0x03:96MB, 0x04:128MB, 0x05:160MB,
 * 0xF0:4MB, 0xF1:8MB, 0xF2:12MB, 0xF3:16MB, 0xF4:20MB, 0xF5:24MB, 0xF6:28MB,
 * 0xF7:32MB, 0xF8:36MB, 0xF9:40MB, 0xFA:44MB, 0xFB:48MB, 0xFC:52MB, 0xFD:56MB,
 * 0xFE:60MB
 *
 * Today all existing SoCs(except Cannonlake) are supported under intel
 * common code block design may not need to use any other values than 0x0-0x05
 * for GFX DSM range. DSM memory ranges between 0xF0-0xF6 are majorly for
 * early SoC samples and validation requirement. This code block to justify
 * all differnet possible ranges that FSP may support for a platform.
 */
size_t sa_get_dsm_size(void)
{
	uint32_t prealloc_memory;
	uint16_t ggc;

	ggc = sa_get_ggc_reg();
	prealloc_memory = (ggc & G_GMS_MASK) >> G_GMS_OFFSET;

	if (prealloc_memory < 0xF0)
		return prealloc_memory * 32*MiB;
	else
		return (prealloc_memory - 0xEF) * 4*MiB;
}

uintptr_t sa_get_gsm_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, BGSM), 1*MiB);
}

size_t sa_get_gsm_size(void)
{
	uint8_t ggms;

	ggms = (sa_get_ggc_reg() & G_GGMS_MASK) >> G_GGMS_OFFSET;

	/*
	 * Size of GSM: 0x0: No Preallocated Memory 0x1: 2MB Memory
	 * 0x2: 4MB Memory 0x3: 8MB Memory
	 */
	if (ggms)
		return 1*MiB << ggms;
	else
		return 0;
}

uintptr_t sa_get_tseg_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, TSEG), 1*MiB);
}

size_t sa_get_tseg_size(void)
{
	return sa_get_gsm_base() - sa_get_tseg_base();
}

/*
 * Get DPR size in case CONFIG_SA_ENABLE_DPR is selected by SoC.
 */
size_t sa_get_dpr_size(void)
{
	uintptr_t dpr_reg;
	size_t size = 0;
	/*
	 * DMA Protected Range can be reserved below TSEG for PCODE patch
	 * or TXT/BootGuard related data.  Rather than report a base address
	 * the DPR register reports the TOP of the region, which is the same
	 * as TSEG base.  The region size is reported in MiB in bits 11:4.
	 */
	dpr_reg = pci_read_config32(SA_DEV_ROOT, DPR);
	if (dpr_reg & DPR_EPM)
		size = (dpr_reg & DPR_SIZE_MASK) << 16;

	return size;
}
