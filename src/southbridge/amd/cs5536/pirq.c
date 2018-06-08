/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#if IS_ENABLED(CONFIG_PIRQ_ROUTE) && IS_ENABLED(CONFIG_GENERATE_PIRQ_TABLE)
void pirq_assign_irqs(const unsigned char pIntAtoD[4])
{
	struct device *pdev;

	pdev = dev_find_device(PCI_VENDOR_ID_AMD,
			       PCI_DEVICE_ID_AMD_CS5536_ISA, 0);

	if (pdev) {
		pci_write_config16(pdev, 0x5c, (pIntAtoD[3] << 12
			| pIntAtoD[2] << 8 | pIntAtoD[1] << 4 | pIntAtoD[0]));
	}
}
#endif
