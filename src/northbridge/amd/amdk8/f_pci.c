/*
 * This file is part of the coreboot project.
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

#ifndef AMDK8_F_PCI_C
#define AMDK8_F_PCI_C

#include "debug.h"

#ifdef UNUSED_CODE
/* bit [10,8] are dev func, bit[1,0] are dev index */
static uint32_t pci_read_config32_index(pci_devfn_t dev, uint32_t index_reg,
		uint32_t index)
{
	uint32_t dword;

	pci_write_config32(dev, index_reg, index);

	dword = pci_read_config32(dev, index_reg+0x4);

	return dword;
}

static void pci_write_config32_index(pci_devfn_t dev, uint32_t index_reg,
		uint32_t index, uint32_t data)
{
	pci_write_config32(dev, index_reg, index);

	pci_write_config32(dev, index_reg + 0x4, data);
}
#endif

uint32_t pci_read_config32_index_wait(pci_devfn_t dev,
		uint32_t index_reg, uint32_t index)
{
	uint32_t dword;

	index &= ~(1<<30);
	pci_write_config32(dev, index_reg, index);

	do {
		dword = pci_read_config32(dev, index_reg);
	} while (!(dword & (1<<31)));

	dword = pci_read_config32(dev, index_reg+0x4);

	return dword;
}

static void pci_write_config32_index_wait(pci_devfn_t dev, uint32_t index_reg,
		uint32_t index, uint32_t data)
{
	uint32_t dword;

	pci_write_config32(dev, index_reg + 0x4, data);

	index |= (1<<30);
	pci_write_config32(dev, index_reg, index);
	do {
		dword = pci_read_config32(dev, index_reg);
	} while (!(dword & (1<<31)));
}

#endif
