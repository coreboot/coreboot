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

#include <arch/io.h>
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/smm.h>
#include <soc/systemagent.h>
#include <stdlib.h>

size_t mmap_region_granluarity(void)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER))
		/* Align to TSEG size when SMM is in use */
		if (CONFIG_SMM_TSEG_SIZE != 0)
			return CONFIG_SMM_TSEG_SIZE;

	/* Make it 8MiB by default. */
	return 8*MiB;
}

/* Returns base of requested region encoded in the system agent. */
static inline uintptr_t system_agent_region_base(size_t reg)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, reg), 1*MiB);
}

static inline uintptr_t smm_region_start(void)
{
	return system_agent_region_base(TSEG);
}

static inline size_t smm_region_size(void)
{
	return system_agent_region_base(BGSM) - smm_region_start();
}

void smm_region(void **start, size_t *size)
{
	*start = (void *)smm_region_start();
	*size = smm_region_size();
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
	const size_t ied_size = CONFIG_IED_REGION_SIZE;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	sub_base = smm_region_start();
	sub_size = smm_region_size();

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

u32 top_of_32bit_ram(void)
{
	msr_t prmrr_base;
	u32 top_of_ram;
	const struct device *dev;
	const struct soc_intel_skylake_config *config;

	/*
	 * Check if Tseg has been initialized, we will use this as a flag
	 * to check if the MRC is done, and only then continue to read the
	 * PRMMR_BASE MSR. The system hangs if PRMRR_BASE MSR is read before
	 * PRMRR_MASK MSR lock bit is set.
	 */
	if (smm_region_start() == 0)
		return 0;

	dev = dev_find_slot(0, PCI_DEVFN(SA_DEV_SLOT_ROOT, 0));
	config = dev->chip_info;

	/*
	 * On Skylake, cbmem_top is offset down from PRMRR_BASE by reserved
	 * memory (128MiB) for CPU trace if enabled, then reserved memory (4KB)
	 * for PTT if enabled. PTT is in fact not used on Skylake platforms.
	 * Refer to Fsp Integration Guide for the memory mapping layout.
	 */
	prmrr_base = rdmsr(UNCORE_PRMRR_PHYS_BASE_MSR);
	top_of_ram = prmrr_base.lo;

	if (config->ProbelessTrace)
		top_of_ram -= TRACE_MEMORY_SIZE;

	return top_of_ram;
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

