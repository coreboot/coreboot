/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <device/pci_ehci.h>
#include <device/pci_def.h>
#include <soc/southbridge.h>

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
	u32 reg32, value;

	value = (port & 0x3) << DEBUG_PORT_SELECT_SHIFT;
	value |= DEBUG_PORT_ENABLE;
	reg32 = pci_read_config32(SOC_EHCI1_DEV, EHCI_HUB_CONFIG4);
	reg32 &= ~DEBUG_PORT_MASK;
	reg32 |= value;
	pci_write_config32(SOC_EHCI1_DEV, EHCI_HUB_CONFIG4, reg32);
}
