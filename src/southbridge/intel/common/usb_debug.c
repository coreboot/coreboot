/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include <device/pci_def.h>

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	u32 class;
	pci_devfn_t dev;

	if (!CONFIG(HAVE_USBDEBUG_OPTIONS))
		return PCI_DEV(0, 0x1d, 7);

	if (hcd_idx == 2)
		dev = PCI_DEV(0, 0x1a, 0);
	else
		dev = PCI_DEV(0, 0x1d, 0);

	/* If we enter here before RCBA programming, EHCI function may
	 * appear with the highest function number instead.
	 */
	class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	if (class != PCI_EHCI_CLASSCODE)
		dev |= PCI_DEV(0, 0, 7);

	return dev;
}

/* Required for successful build, but currently empty. */
void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* Not needed, the ICH* southbridges hardcode physical USB port 1. */
}
