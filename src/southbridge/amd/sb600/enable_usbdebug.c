/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	return  PCI_DEV(0, 0x13, 5); /* USB EHCI, D19:F5 */
}

/* Required for successful build, but currently empty. */
void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* TODO: Allow changing the physical USB port used as Debug Port. */
}

void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, base);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}
