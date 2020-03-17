/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include <types.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include "ironlake.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 pciexbar = 0;
	u32 pciexbar_reg;
	int max_buses;

	pciexbar_reg = pci_read_config32(PCI_DEV(QUICKPATH_BUS, 0, 1), 0x50);

	// MMCFG not supported or not enabled.
	if (!(pciexbar_reg & (1 << 0)))
		return current;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0:		// 256MB
		pciexbar =
		    pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
				    (1 << 28));
		max_buses = 256;
		break;
	case 1:		// 128M
		pciexbar =
		    pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
				    (1 << 28) | (1 << 27));
		max_buses = 128;
		break;
	case 2:		// 64M
		pciexbar =
		    pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
				    (1 << 28) | (1 << 27) | (1 << 26));
		max_buses = 64;
		break;
	default:		// RSVD
		return current;
	}

	if (!pciexbar)
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
					     pciexbar, 0x0, 0x0, max_buses - 1);

	return current;
}
