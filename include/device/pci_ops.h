/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/

#ifndef DEVICE_PCI_OPS_H
#define DEVICE_PCI_OPS_H

#include <types.h>
#include <device/device.h>
#include <pci_ops.h>
#include <shared.h>

u8 pci_read_config8(struct device * dev, unsigned where);
EXPORT_SYMBOL(pci_read_config8);
u16 pci_read_config16(struct device * dev, unsigned where);
EXPORT_SYMBOL(pci_read_config16);
u32 pci_read_config32(struct device * dev, unsigned where);
EXPORT_SYMBOL(pci_read_config32);
void pci_write_config8(struct device * dev, unsigned where, u8 val);
EXPORT_SYMBOL(pci_write_config8);
void pci_write_config16(struct device * dev, unsigned where, u16 val);
EXPORT_SYMBOL(pci_write_config16);
void pci_write_config32(struct device * dev, unsigned where, u32 val);
EXPORT_SYMBOL(pci_write_config32);

u8 pci_conf1_read_config8(u32 bdf, int where);
EXPORT_SYMBOL(pci_conf1_read_config8);
u16 pci_conf1_read_config16(u32 bdf, int where);
EXPORT_SYMBOL(pci_conf1_read_config16);
u32 pci_conf1_read_config32(u32 bdf, int where);
EXPORT_SYMBOL(pci_conf1_read_config32);
void pci_conf1_write_config8(u32 bdf, int where, u8 value);
EXPORT_SYMBOL(pci_conf1_write_config8);
void pci_conf1_write_config16(u32 bdf, int where, u16 value);
EXPORT_SYMBOL(pci_conf1_write_config16);
void pci_conf1_write_config32(u32 bdf, int where, u32 value);
EXPORT_SYMBOL(pci_conf1_write_config32);
int pci_conf1_find_on_bus(u16 bus, u16 vid, u16 did, u32 *busdevfn);
EXPORT_SYMBOL(pci_conf1_find_on_bus);
int pci_conf1_find_device(u16 vid, u16 did, u32 * dev);
EXPORT_SYMBOL(pci_conf1_find_device);

#endif /* DEVICE_PCI_OPS_H */
