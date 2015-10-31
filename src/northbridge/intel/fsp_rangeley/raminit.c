/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <console/console.h>
#include <string.h>
#include <arch/io.h>
#include <cbmem.h>
#include <device/pci_def.h>
#include "northbridge.h"
#include <drivers/intel/fsp1_0/fsp_util.h>

static uintptr_t smm_region_start(void)
{
	/*
	 * Calculate the top of usable (low) DRAM.
	 * The FSP's reserved memory sits just below the SMM region,
	 * allowing calculation of the top of usable memory.
	 */
	uintptr_t tom = sideband_read(B_UNIT, BMBOUND);
	uintptr_t bsmmrrl = sideband_read(B_UNIT, BSMMRRL) << 20;
	if (bsmmrrl) {
		tom = bsmmrrl;
	}

	return tom;
}

void *cbmem_top(void)
{
	return (void *) (smm_region_start() - FSP_RESERVE_MEMORY_SIZE);
}
