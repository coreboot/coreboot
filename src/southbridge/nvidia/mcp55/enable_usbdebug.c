/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include "mcp55.h"

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	return PCI_DEV(0, MCP55_DEVN_BASE + 2, 1); /* USB EHCI */
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	u32 dword;

	/* Write the port number to 0x74[15:12]. */
	dword = pci_read_config32(dev, 0x74);
	dword &= ~(0xf << 12);
	dword |= (port << 12);
	pci_write_config32(dev, 0x74, dword);
}

void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, base);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}
