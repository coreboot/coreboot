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
 */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ehci.h>
#include <device/pci_def.h>

pci_devfn_t pci_ehci_dbg_dev(unsigned int hcd_idx)
{
	u32 class;
	pci_devfn_t dev = PCI_DEV(0, 0x1d, 0);

	class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	if (class != PCI_EHCI_CLASSCODE)
		return 0;

	return dev;
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* Hardcoded to physical port 1 */
}

void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
	u32 tmp32;

	if (!dev)
		return;

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, base);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/* Force ownership of hte Debug Port to the EHCI controller. */
	tmp32 = read32((void *)(base + CONFIG_EHCI_DEBUG_OFFSET));
	tmp32 |= (1 << 30);
	write32((void *)(base + CONFIG_EHCI_DEBUG_OFFSET), tmp32);
}
