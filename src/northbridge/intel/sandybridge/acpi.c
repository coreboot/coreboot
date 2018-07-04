/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors
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
#include <console/console.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "sandybridge.h"
#include <southbridge/intel/bd82x6x/pch.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 pciexbar = 0;
	u32 pciexbar_reg;
	int max_buses;

	struct device *const dev = dev_find_slot(0, PCI_DEVFN(0, 0));

	if (!dev)
		return current;

	pciexbar_reg=pci_read_config32(dev, PCIEXBAR);

	// MMCFG not supported or not enabled.
	if (!(pciexbar_reg & (1 << 0)))
		return current;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		pciexbar = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
		max_buses = 256;
		break;
	case 1: // 128M
		pciexbar = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));
		max_buses = 128;
		break;
	case 2: // 64M
		pciexbar = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26));
		max_buses = 64;
		break;
	default: // RSVD
		return current;
	}

	if (!pciexbar)
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
			pciexbar, 0x0, 0x0, max_buses - 1);

	return current;
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	const struct device *const igfx = dev_find_slot(0, PCI_DEVFN(2, 0));

	if (igfx && igfx->enabled) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current, 0, 0, IOMMU_BASE1);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 1);
		acpi_dmar_drhd_fixup(tmp, current);
	}

	const unsigned long tmp = current;
	current += acpi_create_dmar_drhd(current,
			DRHD_INCLUDE_PCI_ALL, 0, IOMMU_BASE2);
	current += acpi_create_dmar_ds_ioapic(current,
			2, PCH_IOAPIC_PCI_BUS, PCH_IOAPIC_PCI_SLOT, 0);
	size_t i;
	for (i = 0; i < 8; ++i)
		current += acpi_create_dmar_ds_msi_hpet(current,
				0, PCH_HPET_PCI_BUS, PCH_HPET_PCI_SLOT, i);
	acpi_dmar_drhd_fixup(tmp, current);

	return current;
}

unsigned long northbridge_write_acpi_tables(struct device *const dev,
					    unsigned long current,
					    struct acpi_rsdp *const rsdp)
{
	const u32 capid0_a = pci_read_config32(dev, 0xe4);
	if (capid0_a & (1 << 23))
		return current;

	printk(BIOS_DEBUG, "ACPI:     * DMAR\n");
	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
