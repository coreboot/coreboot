/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include "pci.h"

/* bit [10,8] are dev func, bit[1,0] are dev index */

u32 pci_read_config32_index(pci_devfn_t dev, u32 index_reg, u32 index)
{
	u32 dword;

	pci_write_config32(dev, index_reg, index);
	dword = pci_read_config32(dev, index_reg+0x4);
	return dword;
}

#ifdef UNUSED_CODE
void pci_write_config32_index(pci_devfn_t dev, u32 index_reg, u32 index,
		u32 data)
{

	pci_write_config32(dev, index_reg, index);

	pci_write_config32(dev, index_reg + 0x4, data);

}
#endif

u32 pci_read_config32_index_wait(pci_devfn_t dev, u32 index_reg,
		u32 index)
{

	u32 dword;

	index &= ~(1<<30);
	pci_write_config32(dev, index_reg, index);
	do {
		dword = pci_read_config32(dev, index_reg);
	} while (!(dword & (1<<31)));
	dword = pci_read_config32(dev, index_reg+0x4);
	return dword;
}

#ifdef UNUSED_CODE
void pci_write_config32_index_wait(pci_devfn_t dev, u32 index_reg,
		u32 index, u32 data)
{

	u32 dword;

	pci_write_config32(dev, index_reg + 0x4, data);
	index |= (1<<30);
	pci_write_config32(dev, index_reg, index);
	do {
		dword = pci_read_config32(dev, index_reg);
	} while (!(dword & (1<<31)));

}
#endif
