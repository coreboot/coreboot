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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <usbdebug.h>
#include <device/pci_def.h>
#include "mcp55.h"

void set_debug_port(unsigned int port)
{
	u32 dword;
	device_t dev = PCI_DEV(0, MCP55_DEVN_BASE + 2, 1); /* USB EHCI */

	/* Write the port number to 0x74[15:12]. */
	dword = pci_read_config32(dev, 0x74);
	dword &= ~(0xf << 12);
	dword |= (port << 12);
	pci_write_config32(dev, 0x74, dword);
}

void mcp55_enable_usbdebug(unsigned int port)
{
	device_t dev = PCI_DEV(0, MCP55_DEVN_BASE + 2, 1); /* USB EHCI */

	/* Mark the requested physical USB port (1-15) as the Debug Port. */
	set_debug_port(port);

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}
