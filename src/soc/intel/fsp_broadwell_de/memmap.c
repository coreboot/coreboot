/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#include <cpu/x86/smm.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <soc/broadwell_de.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>

void *cbmem_top_chipset(void)
{
	return find_fsp_reserved_mem(*(void **)CBMEM_FSP_HOB_PTR);
}

/*
 * Get TSEG base.
 */
uintptr_t sa_get_tseg_base(void)
{
	const pci_devfn_t dev = PCI_DEV(BUS0, VTD_DEV, VTD_FUNC);

	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(dev, TSEG_BASE), 1 * MiB);
}

size_t sa_get_tseg_size(void)
{
	const pci_devfn_t dev = PCI_DEV(BUS0, VTD_DEV, VTD_FUNC);

	/* All regions concerned for have 1 MiB alignment. */
	size_t ret = ALIGN_DOWN(pci_read_config32(dev, TSEG_LIMIT), 1 * MiB);

	/* Lower 20bit of TSEG_LIMIT are don't care, need to add 1MiB */
	ret += 1 * MiB;

	/* Subtract base to get the size */
	return ret - sa_get_tseg_base();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_get_tseg_base();
	*size = sa_get_tseg_size();
}
