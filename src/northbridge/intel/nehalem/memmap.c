/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google LLC
 * Copyright (C) 2013 Vladimir Serbinenko.
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

#include <arch/cpu.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include <stage_cache.h>
#include <cpu/intel/smm_reloc.h>
#include "nehalem.h"

static uintptr_t smm_region_start(void)
{
	/* Base of TSEG is top of usable DRAM */
	uintptr_t tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return tom;
}

u32 northbridge_get_tseg_base(void)
{
	return (u32)smm_region_start();
}

u32 northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void *cbmem_top(void)
{
	return (void *) smm_region_start();
}

void stage_cache_external_region(void **base, size_t *size)
{
	/* The stage cache lives at the end of TSEG region.
	 * The top of RAM is defined to be the TSEG base address. */
	*size = CONFIG_SMM_RESERVED_SIZE;
	*base = (void *)((uintptr_t)northbridge_get_tseg_base() +
			northbridge_get_tseg_size() - CONFIG_SMM_RESERVED_SIZE);
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB, MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, top_of_ram, 8*MiB, MTRR_TYPE_WRBACK);
}
