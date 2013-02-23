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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#if (CONFIG_PIRQ_ROUTE==1 && CONFIG_GENERATE_PIRQ_TABLE==1)
void pirq_assign_irqs(const unsigned char pIntAtoD[4])
{
	device_t pdev;

	pdev = dev_find_device(PCI_VENDOR_ID_CYRIX,
			       PCI_DEVICE_ID_CYRIX_5530_LEGACY, 0);

	if (pdev) {
		pci_write_config8(pdev, 0x5c, (pIntAtoD[1] << 4 | pIntAtoD[0]));
		pci_write_config8(pdev, 0x5d, (pIntAtoD[3] << 4 | pIntAtoD[2]));
	}
}
#endif
