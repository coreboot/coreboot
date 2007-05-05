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
#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <types.h>
#include <device/device.h>
#include <pci_ops.h>

u8 pci_read_config8(struct device * dev, unsigned where);
u16 pci_read_config16(struct device * dev, unsigned where);
u32 pci_read_config32(struct device * dev, unsigned where);
void pci_write_config8(struct device * dev, unsigned where, u8 val);
void pci_write_config16(struct device * dev, unsigned where, u16 val);
void pci_write_config32(struct device * dev, unsigned where, u32 val);

#endif /* PCI_OPS_H */
