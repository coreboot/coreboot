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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef AMDFAM10_PCI_C
#define AMDFAM10_PCI_C
/* bit [10,8] are dev func, bit[1,0] are dev index */


static u32 pci_read_config32_index(device_t dev, u32 index_reg, u32 index)
{
	u32 dword;

	pci_write_config32(dev, index_reg, index);
	dword = pci_read_config32(dev, index_reg+0x4);
	return dword;
}

#ifdef UNUSED_CODE
static void pci_write_config32_index(device_t dev, u32 index_reg, u32 index, u32 data)
{

	pci_write_config32(dev, index_reg, index);

	pci_write_config32(dev, index_reg + 0x4, data);

}
#endif

static u32 pci_read_config32_index_wait(device_t dev, u32 index_reg, u32 index)
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
static void pci_write_config32_index_wait(device_t dev, u32 index_reg, u32 index, u32 data)
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
#endif


