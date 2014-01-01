/*
 * This file is part of uio_usbdebug
 *
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
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

#include <arch/io.h>
#include <device/device.h>
#include <console/usb.h>

extern void *ehci_bar;

pci_devfn_t pci_ehci_dbg_dev(unsigned hcd_idx)
{
	return 0;
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* claim usb debug port */
	const unsigned long dbgctl_addr =
		((unsigned long)ehci_bar) + CONFIG_EHCI_DEBUG_OFFSET;
	write32(dbgctl_addr, read32(dbgctl_addr) | (1 << 30));
}

void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
}
