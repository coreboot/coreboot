/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <device/pci_ehci.h>
#include <device/pci_def.h>

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	return PCI_DEV(0, 0x1d, 0);
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* Hardcoded to physical port 1 */
}
