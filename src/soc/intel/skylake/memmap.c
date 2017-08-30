/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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
#include <intelblocks/systemagent.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/smm.h>
#include <soc/systemagent.h>
#include <stdlib.h>

size_t mmap_region_granularity(void)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER))
		/* Align to TSEG size when SMM is in use */
		if (CONFIG_SMM_TSEG_SIZE != 0)
			return CONFIG_SMM_TSEG_SIZE;

	/* Make it 8MiB by default. */
	return 8*MiB;
}

void smm_region(void **start, size_t *size)
{
	*start = (void *)sa_get_tseg_base();
	*size = sa_get_tseg_size();
}

/*
 *        Subregions within SMM
 *     +-------------------------+ BGSM
 *     |          IED            | IED_REGION_SIZE
 *     +-------------------------+
 *     |  External Stage Cache   | SMM_RESERVED_SIZE
 *     +-------------------------+
 *     |      code and data      |
 *     |         (TSEG)          |
 *     +-------------------------+ TSEG
 */
int smm_subregion(int sub, void **start, size_t *size)
{
	uintptr_t sub_base;
	size_t sub_size;
	void *smm_base;
	const size_t ied_size = CONFIG_IED_REGION_SIZE;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	smm_region(&smm_base, &sub_size);
	sub_base = (uintptr_t)smm_base;

	switch (sub) {
	case SMM_SUBREGION_HANDLER:
		/* Handler starts at the base of TSEG. */
		sub_size -= ied_size;
		sub_size -= cache_size;
		break;
	case SMM_SUBREGION_CACHE:
		/* External cache is in the middle of TSEG. */
		sub_base += sub_size - (ied_size + cache_size);
		sub_size = cache_size;
		break;
	case SMM_SUBREGION_CHIPSET:
		/* IED is at the top. */
		sub_base += sub_size - ied_size;
		sub_size = ied_size;
		break;
	default:
		return -1;
	}

	*start = (void *)sub_base;
	*size = sub_size;

	return 0;
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
static u32 calculate_dram_base(void)
{
	const struct soc_intel_skylake_config *config;
	const struct device *dev;
	uint32_t dram_base;
	uint32_t prmrr_base;
	size_t prmrr_size;

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

	dev = dev_find_slot(0, PCI_DEVFN(SA_DEV_SLOT_ROOT, 0));
	config = dev->chip_info;
	if (IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1))
		prmrr_size = 1*MiB;
	else
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
	}

	if (config->ProbelessTrace) {
		/* GDXC MOT */
		dram_base -= GDXC_MOT_MEMORY_SIZE;
		/* Round down to natual boundary accroding to PSMI size */
		dram_base = ALIGN_DOWN(dram_base, PSMI_BUFFER_AREA_SIZE);
		/* GDXC IOT */
		dram_base -= GDXC_IOT_MEMORY_SIZE;
		/* PSMI buffer area */
		dram_base -= PSMI_BUFFER_AREA_SIZE;
	}

	if (is_ptt_enable())
		dram_base -= 4*KiB; /* Allocate 4KB for PTT if enable */

	return dram_base;
}

/* Get usable system memory start address */
static u32 top_of_32bit_ram(void)
{
	/*
	 * Check if Tseg has been initialized, we will use this as a flag
	 * to check if the MRC is done, and only then continue to read the
	 * PRMMR_BASE MSR. The system hangs if PRMRR_BASE MSR is read before
	 * PRMRR_MASK MSR lock bit is set.
	 */
	if (sa_get_tseg_base() == 0)
		return 0;

	return calculate_dram_base();
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
	return (void *)top_of_32bit_ram();
}
