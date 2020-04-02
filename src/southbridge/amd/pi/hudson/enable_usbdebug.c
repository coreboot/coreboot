/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ehci.h>
#include <device/pci_def.h>
#include "hudson.h"

#define DEBUGPORT_MISC_CONTROL		0x80

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	/* Enable all of the USB controllers */
	outb(0xEF, PM_INDEX);
	outb(0x7F, PM_DATA);

	if (hcd_idx == 3)
		return PCI_DEV(0, 0x16, 0);
	else if (hcd_idx == 2)
		return PCI_DEV(0, 0x13, 0);
	else
		return PCI_DEV(0, 0x12, 0);
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	u8 *base_regs = pci_ehci_base_regs(dev);
	u32 reg32;

	/* Write the port number to DEBUGPORT_MISC_CONTROL[31:28]. */
	reg32 = read32(base_regs + DEBUGPORT_MISC_CONTROL);
	reg32 &= ~(0xf << 28);
	reg32 |= (port << 28);
	reg32 |= (1 << 27); /* Enable Debug Port port number remapping. */
	write32(base_regs + DEBUGPORT_MISC_CONTROL, reg32);
}
