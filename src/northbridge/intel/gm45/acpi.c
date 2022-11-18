/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include "gm45.h"

static unsigned long acpi_fill_dmar(unsigned long current)
{
	const struct device *dev;

	dev = pcidev_on_root(3, 0);
	int me_active = dev && dev->enabled;

	dev = pcidev_on_root(2, 0);
	int igd_active = dev && dev->enabled;

	int stepping = pci_read_config8(pcidev_on_root(0, 0),
							   PCI_CLASS_REVISION);

	unsigned long tmp = current;
	current += acpi_create_dmar_drhd(current, 0, 0, IOMMU_BASE1);
	current += acpi_create_dmar_ds_pci(current, 0, 0x1b, 0);
	acpi_dmar_drhd_fixup(tmp, current);

	if (stepping != STEPPING_B2 && igd_active) {
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

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	acpi_dmar_t *dmar;

	current = start;

	printk(BIOS_DEBUG, "ACPI:     * DMAR\n");
	dmar = (acpi_dmar_t *)current;
	acpi_create_dmar(dmar, 0, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
