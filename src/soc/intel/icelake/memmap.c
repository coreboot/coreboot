/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
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

#include <arch/romstage.h>
#include <arch/ebda.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <intelblocks/ebda.h>
#include <intelblocks/systemagent.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <stdlib.h>

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_get_tseg_base();
	*size = sa_get_tseg_size();
}

/* Calculate ME Stolen size */
static size_t get_imr_size(void)
{
	size_t imr_size;

	/* ME stolen memory */
	imr_size = MCHBAR32(IMRLIMIT) - MCHBAR32(IMRBASE);

	return imr_size;
}

/* Calculate PRMRR size based on user input PRMRR size and alignment */
static size_t get_prmrr_size(uintptr_t dram_base,
		const struct soc_intel_icelake_config *config)
{
	uintptr_t prmrr_base = dram_base;
	size_t prmrr_size;

	prmrr_size = config->PrmrrSize;

	/* Allocate PRMRR memory for C6DRAM */
	if (!prmrr_size) {
		if (config->enable_c6dram)
			prmrr_size = 1*MiB;
		else
			return 0;
	}

	/*
	 * PRMRR Sizes that are > 1MB and < 32MB are
	 * not supported and will fail out.
	 */
	if ((prmrr_size > 1*MiB) && (prmrr_size < 32*MiB))
		die("PRMRR Sizes that are > 1MB and < 32MB are not"
				"supported!\n");

	prmrr_base -= prmrr_size;
	if (prmrr_size >= 32*MiB)
		prmrr_base = ALIGN_DOWN(prmrr_base, 128*MiB);
	else
		prmrr_base = ALIGN_DOWN(prmrr_base, 16*MiB);
	/* PRMRR Area Size */
	prmrr_size = dram_base - prmrr_base;

	return prmrr_size;
}

/* Calculate Intel Traditional Memory size based on GSM, DSM, TSEG and DPR. */
static size_t calculate_traditional_mem_size(uintptr_t dram_base,
		const struct device *dev)
{
	uintptr_t traditional_mem_base = dram_base;
	size_t traditional_mem_size;

	if (dev->enabled) {
		/* Read BDSM from Host Bridge */
		traditional_mem_base -= sa_get_dsm_size();

		/* Read BGSM from Host Bridge */
		traditional_mem_base -= sa_get_gsm_size();
	}
	/* Get TSEG size */
	traditional_mem_base -= sa_get_tseg_size();

	/* Get DPR size */
	if (CONFIG(SA_ENABLE_DPR))
		traditional_mem_base -= sa_get_dpr_size();

	/* Traditional Area Size */
	traditional_mem_size = dram_base - traditional_mem_base;

	return traditional_mem_size;
}

/*
 * Calculate Intel Reserved Memory size based on
 * PRMRR size, Me stolen memory and PTT selection.
 */
static size_t calculate_reserved_mem_size(uintptr_t dram_base,
		const struct device *dev)
{
	uintptr_t reserve_mem_base = dram_base;
	size_t reserve_mem_size;
	const struct soc_intel_icelake_config *config;

	config = config_of(dev);

	/* Get PRMRR size */
	reserve_mem_base -= get_prmrr_size(reserve_mem_base, config);

	/* Get Tracehub size */
	reserve_mem_base -= get_imr_size();

	/* Traditional Area Size */
	reserve_mem_size = dram_base - reserve_mem_base;

	return reserve_mem_size;
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
static uintptr_t calculate_dram_base(size_t *reserved_mem_size)
{
	uintptr_t dram_base;
	const struct device *dev;

	dev = pcidev_on_root(SA_DEV_SLOT_IGD, 0);
	if (!dev)
		die_with_post_code(POST_HW_INIT_FAILURE,
				   "ERROR - IGD device not found!");

	/* Read TOLUD from Host Bridge offset */
	dram_base = sa_get_tolud_base();

	/* Get Intel Traditional Memory Range Size */
	dram_base -= calculate_traditional_mem_size(dram_base, dev);

	/* Get Intel Reserved Memory Range Size */
	*reserved_mem_size = calculate_reserved_mem_size(dram_base, dev);

	dram_base -= *reserved_mem_size;

	return dram_base;
}

/*
 * SoC implementation
 *
 * SoC call to summarize all Intel Reserve MMIO size and report to SA
 */
size_t soc_reserved_mmio_size(void)
{
	struct ebda_config cfg;

	retrieve_ebda_object(&cfg);

	/* Get Intel Reserved Memory Range Size */
	return cfg.reserved_mem_size;
}

/* Fill up memory layout information */
void fill_soc_memmap_ebda(struct ebda_config *cfg)
{
	size_t chipset_mem_size;

	cfg->tolum_base = calculate_dram_base(&chipset_mem_size);
	cfg->reserved_mem_size = chipset_mem_size;
}

void cbmem_top_init(void)
{
	/* Fill up EBDA area */
	fill_ebda_area();
}

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
void *cbmem_top(void)
{
	struct ebda_config ebda_cfg;

	/*
	 * Check if Tseg has been initialized, we will use this as a flag
	 * to check if the MRC is done, and only then continue to read the
	 * PRMMR_BASE MSR. The system hangs if PRMRR_BASE MSR is read before
	 * PRMRR_MASK MSR lock bit is set.
	 */
	if (sa_get_tseg_base() == 0)
		return NULL;

	retrieve_ebda_object(&ebda_cfg);

	return (void *)(uintptr_t)ebda_cfg.tolum_base;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;
	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs under cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	top_of_ram -= 16*MiB;
	postcar_frame_add_mtrr(pcf, top_of_ram, 16*MiB, MTRR_TYPE_WRBACK);
}
