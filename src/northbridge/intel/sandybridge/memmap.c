/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <program_loading.h>
#include "sandybridge.h"

static uintptr_t smm_region_start(void)
{
	/* Base of TSEG is top of usable DRAM */
	uintptr_t tom = pci_read_config32(PCI_DEV(0, 0, 0), TSEGMB);
	return tom;
}

void *cbmem_top_chipset(void)
{
	return (void *) smm_region_start();
}

static uintptr_t northbridge_get_tseg_base(void)
{
	return ALIGN_DOWN(smm_region_start(), 1*MiB);
}

static size_t northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	top_of_ram = (uintptr_t)cbmem_top();
	/* Cache 8MiB below the top of ram. On sandybridge systems the top of
	 * ram under 4GiB is the start of the TSEG region. It is required to
	 * be 8MiB aligned. Set this area as cacheable so it can be used later
	 * for ramstage before setting up the entire RAM as cacheable. */
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB, MTRR_TYPE_WRBACK);

	/* Cache 8MiB at the top of ram. Top of ram on sandybridge systems
	 * is where the TSEG region resides. However, it is not restricted
	 * to SMM mode until SMM has been relocated. By setting the region
	 * to cacheable it provides faster access when relocating the SMM
	 * handler as well as using the TSEG region for other purposes. */
	postcar_frame_add_mtrr(pcf, top_of_ram, 8*MiB, MTRR_TYPE_WRBACK);
}
