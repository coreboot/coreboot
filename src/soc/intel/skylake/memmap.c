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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <cbmem.h>
#include <device/pci.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/smm.h>
#include <soc/systemagent.h>

size_t mmap_region_granluarity(void)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER))
		/* Align to TSEG size when SMM is in use */
		if (CONFIG_SMM_TSEG_SIZE != 0)
			return CONFIG_SMM_TSEG_SIZE;

	/* Make it 8MiB by default. */
	return 8 << 20;
}

static void *smm_region_start(void)
{
	/*
	 * SMM base address matches the top of DPR.  The DPR register has
	 * 1 MiB alignment and reports the TOP of the DPR range.
	 */
	uint32_t smm_base = pci_read_config32(SA_DEV_ROOT, DPR);
	smm_base = ALIGN_DOWN(smm_base, 1 << 20);
	return (void *)smm_base;
}

void smm_region(void **start, size_t *size)
{
	*start = smm_region_start();
	*size = mmap_region_granluarity();
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
	 *     | Chipset Reserved Memory |  Length: Multiple of CONFIG_TSEG_SIZE
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

	unsigned long top_of_ram = (unsigned long)smm_region_start();

	/*
	 * Subtract DMA Protected Range size if enabled and align to a multiple
	 * of TSEG size.
	 */
	u32 dpr = pci_read_config32(SA_DEV_ROOT, DPR);
	if (dpr & DPR_EPM) {
		top_of_ram -= (dpr & DPR_SIZE_MASK) << 16;
		top_of_ram = ALIGN_DOWN(top_of_ram, mmap_region_granluarity());
	}

	return (void *)top_of_ram;
}

