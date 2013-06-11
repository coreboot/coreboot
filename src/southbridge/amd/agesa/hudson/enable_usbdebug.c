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
#include <stdint.h>
#include <arch/io.h>
#include <usbdebug.h>
#include <device/pci_def.h>
#include "hudson.h"

#ifndef HUDSON_DEVN_BASE
#define HUDSON_DEVN_BASE 0
#endif

#define EHCI_EOR		(CONFIG_EHCI_BAR + 0x20)
#define DEBUGPORT_MISC_CONTROL	(EHCI_EOR + 0x80)

simple_device_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	return PCI_DEV(0, HUDSON_DEVN_BASE + 0x12, 2);
}

void set_debug_port(unsigned int port)
{
	u32 reg32;

	/* Write the port number to DEBUGPORT_MISC_CONTROL[31:28]. */
	reg32 = read32(DEBUGPORT_MISC_CONTROL);
	reg32 &= ~(0xf << 28);
	reg32 |= (port << 28);
	reg32 |= (1 << 27); /* Enable Debug Port port number remapping. */
	write32(DEBUGPORT_MISC_CONTROL, reg32);
}


void enable_usbdebug(unsigned int hcd_idx, unsigned int port)
{
	simple_device_t dev = pci_ehci_dbg_dev(hcd_idx);

	/* Enable all of the USB controllers */
	outb(0xEF, PM_INDEX);
	outb(0x7F, PM_DATA);

	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
	set_debug_port(port);
}
