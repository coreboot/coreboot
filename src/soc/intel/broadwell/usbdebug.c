/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <usbdebug.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <soc/pci_devs.h>

void set_debug_port(unsigned int port)
{
	/* Hardcoded to physical port 1 */
}

void enable_usbdebug(unsigned int port)
{
	u32 tmp32;

	tmp32 = pci_read_config32(PCH_DEV_EHCI, PCI_VENDOR_ID);
	if (tmp32 == 0xffffffff || tmp32 == 0)
		return;

	/* Set the EHCI BAR address. */
	pci_write_config32(PCH_DEV_EHCI, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(PCH_DEV_EHCI, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/* Force ownership of the Debug Port to the EHCI controller. */
	tmp32 = read32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET);
	tmp32 |= (1 << 30);
	write32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET, tmp32);
}
