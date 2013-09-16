/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <usbdebug.h>
#include <device/pci_def.h>
#include "sb800.h"

#define DEBUGPORT_MISC_CONTROL		0x80

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	if (hcd_idx==3)
		return PCI_DEV(0, 0x16, 2);
	else if (hcd_idx==2)
		return PCI_DEV(0, 0x13, 2);
	else
		return PCI_DEV(0, 0x12, 2);
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	u32 base_regs = pci_ehci_base_regs(dev);
	u32 reg32;

	/* Write the port number to DEBUGPORT_MISC_CONTROL[31:28]. */
	reg32 = read32(base_regs + DEBUGPORT_MISC_CONTROL);
	reg32 &= ~(0xf << 28);
	reg32 |= (port << 28);
	reg32 |= (1 << 27); /* Enable Debug Port port number remapping. */
	write32(base_regs + DEBUGPORT_MISC_CONTROL, reg32);
}


void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
	/* Enable all of the USB controllers */
	outb(0xEF, PM_INDEX);
	outb(0x7F, PM_DATA);

	pci_write_config32(dev, EHCI_BAR_INDEX, base);
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
}
