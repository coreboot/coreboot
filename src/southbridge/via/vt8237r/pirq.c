/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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
#include <pc80/i8259.h>

#if IS_ENABLED(CONFIG_PIRQ_ROUTE) && IS_ENABLED(CONFIG_GENERATE_PIRQ_TABLE)
void pirq_assign_irqs(const unsigned char route[4])
{
	struct device *pdev;

	pdev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237R_LPC, 0);
	if (!pdev)
		pdev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237S_LPC, 0);
	if (!pdev)
		pdev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237A_LPC, 0);
	if (!pdev)
		return;

	pci_write_config8(pdev, 0x55,  route[0] << 4);
	pci_write_config8(pdev, 0x56, (route[2] << 4) | route[1]);
	pci_write_config8(pdev, 0x57,  route[3] << 4);

	/* Enable INT[E-H] mapped to INT[A-D] for simplicity */
	pci_write_config8(pdev, 0x46,  0x00);
}
#endif
