/*
 * This file is part of the coreboot project.
 *
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
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <program_loading.h>
#include <cpu/intel/smm_reloc.h>
#include "ironlake.h"

static uintptr_t smm_region_start(void)
{
	/* Base of TSEG is top of usable DRAM */
	uintptr_t tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return tom;
}

static uintptr_t northbridge_get_tseg_base(void)
{
	return smm_region_start();
}

static size_t northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void *cbmem_top_chipset(void)
{
	return (void *) smm_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_size();
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
