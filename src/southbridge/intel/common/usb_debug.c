/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef __PRE_RAM__
#define __PRE_RAM__ // Use simple device model for this file even in ramstage
#endif
#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <usbdebug.h>
#include <device/pci_def.h>

unsigned int pci_ehci_dbg_dev(unsigned int hcd_idx)
{
#if CONFIG_HAVE_USBDEBUG_OPTIONS
	if (hcd_idx==0)
		return PCI_DEV(0, 0x1a, 0);
	else
		return PCI_DEV(0, 0x1d, 0);
#else
	return PCI_DEV(0, 0x1d, 7);
#endif
}

/* Required for successful build, but currently empty. */
void set_debug_port(unsigned int port)
{
	/* Not needed, the ICH* southbridges hardcode physical USB port 1. */
}

void enable_usbdebug(unsigned int ehci_idx, unsigned int port)
{
	u32 dbgctl, class;
	device_t dev = pci_ehci_dbg_dev(ehci_idx);

	class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	if (class != PCI_EHCI_CLASSCODE) {
		/* If we enter here before RCBA programming, EHCI function may
		 * appear with the highest function number instead.
		 */
		dev |= PCI_DEV(0, 0, 7);
		class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	}

	/* Bail out. No console to complain in. */
	if (class != PCI_EHCI_CLASSCODE)
		return;

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/* Force ownership of the Debug Port to the EHCI controller. */
	dbgctl = read32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET);
	dbgctl |= (1 << 30);
	write32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET, dbgctl);
}
