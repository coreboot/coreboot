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

#include <arch/io.h>
#include <cbmem.h>
#include <device/pci.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

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
