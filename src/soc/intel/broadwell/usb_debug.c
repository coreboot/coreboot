/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

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
