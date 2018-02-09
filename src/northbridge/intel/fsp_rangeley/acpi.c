/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include "northbridge.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct device *dev;
	u32 pciexbar = 0;
	u32 pciexbar_reg;
	int max_buses;
	int pci_dev_id;

	for (pci_dev_id = PCI_DEVICE_ID_RG_MIN; pci_dev_id <= PCI_DEVICE_ID_RG_MAX; pci_dev_id++) {
		dev = dev_find_device(PCI_VENDOR_ID_INTEL, pci_dev_id, 0);
		if (dev)
			break;
	}

	if (!dev)
		return current;

	pciexbar_reg = sideband_read(B_UNIT, BECREG);

	/* MMCFG not supported or not enabled. */
	if (!(pciexbar_reg & (1 << 0)))
		return current;

	/* 256MB ECAM range */
	pciexbar = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
	max_buses = 256;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
			pciexbar, 0x0, 0x0, max_buses - 1);

	return current;
}

void northbridge_acpi_fill_ssdt_generator(struct device *device)
{
	u32 bmbound;
	char pscope[] = "\\_SB.PCI0";

	bmbound = sideband_read(B_UNIT, BMBOUND);
	acpigen_write_scope(pscope);
	acpigen_write_name_dword("BMBD", bmbound);
	acpigen_pop_len();
	generate_cpu_entries(device);
}
