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

#include <arch/romstage.h>
#include <arch/ebda.h>
#include <device/mmio.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/ebda.h>
#include <intelblocks/systemagent.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <stdlib.h>

#include "chip.h"

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_get_tseg_base();
	*size = sa_get_tseg_size();
}

static bool is_ptt_enable(void)
{
	if ((read32((void *)PTT_TXT_BASE_ADDRESS) & PTT_PRESENT) ==
			PTT_PRESENT)
		return true;

	return false;
}

/* Calculate PTT size */
static size_t get_ptt_size(void)
{
	/* Allocate 4KB for PTT if enabled */
	return is_ptt_enable() ? 4*KiB : 0;
}

/* Calculate Trace Hub size */
static size_t get_tracehub_size(uintptr_t dram_base,
		const struct soc_intel_skylake_config *config)
{
	uintptr_t tracehub_base = dram_base;
	size_t tracehub_size = 0;

	if (!config->ProbelessTrace)
		return 0;

	/* GDXC MOT */
	tracehub_base -= GDXC_MOT_MEMORY_SIZE;
	/* Round down to natural boundary according to PSMI size */
	tracehub_base = ALIGN_DOWN(tracehub_base, PSMI_BUFFER_AREA_SIZE);
	/* GDXC IOT */
	tracehub_base -= GDXC_IOT_MEMORY_SIZE;
	/* PSMI buffer area */
	tracehub_base -= PSMI_BUFFER_AREA_SIZE;

	/* Tracehub Area Size */
	tracehub_size = dram_base - tracehub_base;

	return tracehub_size;
}

/* Calculate PRMRR size based on user input PRMRR size and alignment */
static size_t get_prmrr_size(uintptr_t dram_base,
		const struct soc_intel_skylake_config *config)
{
	uintptr_t prmrr_base = dram_base;
	size_t prmrr_size;

	if (CONFIG(PLATFORM_USES_FSP1_1))
		prmrr_size = 1*MiB;
	else
		prmrr_size = config->PrmrrSize;

	if (!prmrr_size)
		return 0;

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

	/* PRMRR Area Size */
	prmrr_size = dram_base - prmrr_base;

	return prmrr_size;
}

/* Calculate Intel Traditional Memory size based on GSM, DSM, TSEG and DPR. */
static size_t calculate_traditional_mem_size(uintptr_t dram_base)
{
	const struct device *igd_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	uintptr_t traditional_mem_base = dram_base;
	size_t traditional_mem_size;

	if (igd_dev && igd_dev->enabled) {
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
 * PRMRR size, Trace Hub config and PTT selection.
 */
static size_t calculate_reserved_mem_size(uintptr_t dram_base)
{
	const struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	uintptr_t reserve_mem_base = dram_base;
	size_t reserve_mem_size;
	const struct soc_intel_skylake_config *config;

	config = config_of(dev);

	/* Get PRMRR size */
	reserve_mem_base -= get_prmrr_size(reserve_mem_base, config);

	/* Get Tracehub size */
	reserve_mem_base -= get_tracehub_size(reserve_mem_base, config);

	/* Get PTT size */
	reserve_mem_base -= get_ptt_size();

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
 * |     Trace Memory         |
 * +--------------------------+ Probless Trace
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

	/* Read TOLUD from Host Bridge offset */
	dram_base = sa_get_tolud_base();

	/* Get Intel Traditional Memory Range Size */
	dram_base -= calculate_traditional_mem_size(dram_base);

	/* Get Intel Reserved Memory Range Size */
	*reserved_mem_size = calculate_reserved_mem_size(dram_base);

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

#if CONFIG(PLATFORM_USES_FSP2_0)
void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;
	uintptr_t smm_base;
	size_t smm_size;

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

	/*
	 * Cache the TSEG region at the top of ram. This region is
	 * not restricted to SMM mode until SMM has been relocated.
	 * By setting the region to cacheable it provides faster access
	 * when relocating the SMM handler as well as using the TSEG
	 * region for other purposes.
	 */
	smm_region(&smm_base, &smm_size);
	postcar_frame_add_mtrr(pcf, smm_base, smm_size, MTRR_TYPE_WRBACK);
}
#endif
