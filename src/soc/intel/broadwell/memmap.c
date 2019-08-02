/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#define __SIMPLE_DEVICE__

#include <cbmem.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <soc/smm.h>
#include <stage_cache.h>
#include <stdint.h>

static uintptr_t dpr_region_start(void)
{
	/*
	 * Base of DPR is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignment and reports the TOP of the range, the base
	 * must be calculated from the size in MiB in bits 11:4.
	 */
	uintptr_t dpr = pci_read_config32(SA_DEV_ROOT, DPR);
	uintptr_t tom = dpr & ~((1 << 20) - 1);

	/* Subtract DMA Protected Range size if enabled */
	if (dpr & DPR_EPM)
		tom -= (dpr & DPR_SIZE_MASK) << 16;

	return tom;
}

void *cbmem_top(void)
{
	return (void *) dpr_region_start();
}

void stage_cache_external_region(void **base, size_t *size)
{
	/* The ramstage cache lives in the TSEG region.
	 * The top of RAM is defined to be the TSEG base address. */
	u32 offset = smm_region_size();
	offset -= CONFIG_IED_REGION_SIZE;
	offset -= CONFIG_SMM_RESERVED_SIZE;

	*base = (void *)(cbmem_top() + offset);
	*size = CONFIG_SMM_RESERVED_SIZE;
}
