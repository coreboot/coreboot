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

/* Use simple device model for this file even in ramstage */
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <commonlib/helpers.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include "haswell.h"

static uintptr_t smm_region_start(void)
{
	/*
	 * Base of TSEG is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignment.
	 */
	uintptr_t tom = pci_read_config32(HOST_BRIDGE, TSEG);
	return tom & ~((1 << 20) - 1);
}

void *cbmem_top_chipset(void)
{
	return (void *)smm_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = smm_region_start();
	*size = CONFIG_SMM_TSEG_SIZE;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8 * MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);
}
