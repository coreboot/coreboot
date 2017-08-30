/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <intelblocks/systemagent.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <stdlib.h>

static void *top_of_ram_register(void)
{
	int num;
	int offset;
	num = (read32((uintptr_t *)HPET_BASE_ADDRESS) >> 8) & 0x1f;
	offset = 0x100 + (0x20 * num) + 0x08;
	return (void *)(uintptr_t)(HPET_BASE_ADDRESS + offset);
}

void clear_cbmem_top(void)
{
	write32(top_of_ram_register(), 0);
}

static bool is_ptt_enable(void)
{
	if ((read32((void *)PTT_TXT_BASE_ADDRESS) & PTT_PRESENT) ==
			PTT_PRESENT)
		return true;

	return false;
}

/*
 * Host Memory Map:
 *
 * +--------------------------+ TOUUD
 * |                          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ TOLUD (also maps into MC address space)
 * |     iGD                  |
 * +--------------------------+ BDSM
 * |     GTT                  |
 * +--------------------------+ BGSM
 * |     TSEG                 |
 * +--------------------------+ TSEGMB
 * |   DMA Protected Region   |
 * +--------------------------+ DPR
 * |    PRM (C6DRAM/SGX)      |
 * +--------------------------+ PRMRR
 * |     ME Stolen Memory     |
 * +--------------------------+ ME Stolen
 * |     PTT                  |
 * +--------------------------+ top_of_ram
 * |     Reserved - FSP/CBMEM |
 * +--------------------------+ TOLUM
 * |     Usage DRAM           |
 * +--------------------------+ 0
 *
 * Some of the base registers above can be equal making the size of those
 * regions 0. The reason is because the memory controller internally subtracts
 * the base registers from each other to determine sizes of the regions. In
 * other words, the memory map is in a fixed order no matter what.
 */
static uintptr_t calculate_dram_base(void)
{
	const struct soc_intel_cannonlake_config *config;
	const struct device *dev;
	uintptr_t dram_base;
	uintptr_t prmrr_base;
	size_t prmrr_size;
	size_t imr_size;

	dev = dev_find_slot(0, PCI_DEVFN(SA_DEV_SLOT_IGD, 0));

	/* Read TOLUD from Host Bridge offset */
	dram_base = sa_get_tolud_base();

	if (dev->enabled) {
		/* Read BDSM from Host Bridge */
		dram_base -= sa_get_dsm_size();

		/* Read BGSM from Host Bridge */
		dram_base -= sa_get_gsm_size();
	}
	/* Get TSEG size */
	dram_base -= sa_get_tseg_size();

	/* Get DPR size */
	if (IS_ENABLED(CONFIG_SA_ENABLE_DPR))
		dram_base -= sa_get_dpr_size();

	config = dev->chip_info;
	prmrr_size = config->PrmrrSize;

	if (prmrr_size > 0) {
		/*
		 * PRMRR Sizes that are > 1MB and < 32MB are
		 * not supported and will fail out.
		 */
		if ((prmrr_size > 1*MiB) && (prmrr_size < 32*MiB))
			die("PRMRR Sizes that are > 1MB and < 32MB are not"
					"supported!\n");

		prmrr_base = dram_base - prmrr_size;
		if (prmrr_size >= 32*MiB)
			prmrr_base = ALIGN_DOWN(prmrr_base, 128*MiB);
		dram_base = prmrr_base;
	} else if (config->enable_c6dram && prmrr_size == 0) {
		/* Allocate PRMRR memory for C6DRAM */
		dram_base -= 1*MiB;
	}

	/* ME stolen memory */
	imr_size = MCHBAR32(IMRLIMIT) - MCHBAR32(IMRBASE);
	if (imr_size > 0)
		dram_base -= imr_size;

	if (is_ptt_enable())
		dram_base -= 4*KiB; /* Allocate 4KB for PTT if enable */

	return dram_base;
}

void cbmem_top_init(void)
{
	uintptr_t top;

	top = calculate_dram_base();

	write32(top_of_ram_register(), top);
}

void *cbmem_top(void)
{
	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |
	 *     |                         |
	 *     +-------------------------+  top_of_ram (aligned)
	 *     |                         |
	 *     |       CBMEM Root        |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |   FSP Reserved Memory   |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |  Various CBMEM Entries  |
	 *     |                         |
	 *     +-------------------------+  top_of_stack (8 byte aligned)
	 *     |                         |
	 *     |   stack (CBMEM Entry)   |
	 *     |                         |
	 *     +-------------------------+
	 */
	return (void *)(uintptr_t)read32(top_of_ram_register());
}
