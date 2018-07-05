/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cbmem.h>
#include <arch/acpigen.h>
#include <cpu/cpu.h>
#include "gm45.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct device *dev;
	u32 pciexbar = 0;
	u32 pciexbar_reg;
	int max_buses;

	dev = dev_find_device(0x8086, 0x2a40, 0);
	if (!dev)
		return current;

	pciexbar_reg = pci_read_config32(dev, D0F0_PCIEXBAR_LO);

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
	int me_active = (dev_find_slot(0, PCI_DEVFN(3, 0)) != NULL) &&
		(pci_read_config8(dev_find_slot(0, PCI_DEVFN(3, 0)), PCI_CLASS_REVISION) != 0xff);
	int stepping = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), PCI_CLASS_REVISION);

	unsigned long tmp = current;
	current += acpi_create_dmar_drhd(current, 0, 0, IOMMU_BASE1);
	current += acpi_create_dmar_ds_pci(current, 0, 0x1b, 0);
	acpi_dmar_drhd_fixup(tmp, current);

	if (stepping != STEPPING_B2) {
		tmp = current;
		current += acpi_create_dmar_drhd(current, 0, 0, IOMMU_BASE2);
		current += acpi_create_dmar_ds_pci(current, 0, 0x2, 0);
		current += acpi_create_dmar_ds_pci(current, 0, 0x2, 1);
		acpi_dmar_drhd_fixup(tmp, current);
	}

	if (me_active) {
		tmp = current;
		current += acpi_create_dmar_drhd(current, 0, 0, IOMMU_BASE3);
		current += acpi_create_dmar_ds_pci(current, 0, 0x3, 0);
		current += acpi_create_dmar_ds_pci(current, 0, 0x3, 1);
		current += acpi_create_dmar_ds_pci(current, 0, 0x3, 2);
		current += acpi_create_dmar_ds_pci(current, 0, 0x3, 3);
		acpi_dmar_drhd_fixup(tmp, current);
	}

	current += acpi_create_dmar_drhd(current, DRHD_INCLUDE_PCI_ALL, 0, IOMMU_BASE4);

	/* TODO: reserve GTT for 0.2.0 and 0.2.1? */
	return current;
}

unsigned long northbridge_write_acpi_tables(struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	acpi_dmar_t *dmar;

	current = start;

	printk(BIOS_DEBUG, "ACPI:     * DMAR\n");
	dmar = (acpi_dmar_t *) current;
	acpi_create_dmar(dmar, 0, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
