/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
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

#include <arch/acpigen.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <northbridge/intel/pineview/pineview.h>
#include <types.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	device_t dev;
	u32 pciexbar = 0;
	u32 pciexbar_reg;
	u32 reg32;
	int max_buses;
	const struct {
		u16 num_buses;
		u32 addr_mask;
	} busmask[] = {
		{256, 0xff000000},
		{128, 0xf8000000},
		{64,  0xfc000000},
		{0,   0},
	};

	dev = dev_find_slot(0, PCI_DEVFN(0,0));
	if (!dev)
		return current;

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	// MMCFG not supported or not enabled.
	if (!(pciexbar_reg & (1 << 0))) {
		printk(BIOS_WARNING, "WARNING: MMCONF not set\n");
		return current;
	}

	reg32 = (pciexbar_reg >> 1) & 3;
	pciexbar = pciexbar_reg & busmask[reg32].addr_mask;
	max_buses = busmask[reg32].num_buses;

	if (!pciexbar) {
		printk(BIOS_WARNING, "WARNING: pciexbar invalid\n");
		return current;
	}

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
			pciexbar, 0x0, 0x0, max_buses - 1);

	return current;
}
