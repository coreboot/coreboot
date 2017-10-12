/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2017 Intel Corp.
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

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <drivers/intel/gma/opregion.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	device_t dev = SA_DEV_IGD;

	/* Check if IGD PCI device is disabled */
	if (!dev->enabled)
		return 0;

	return find_resource(dev, PCI_BASE_ADDRESS_2)->base;
}

static unsigned long igd_write_opregion(device_t dev, unsigned long current,
				struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;

	printk(BIOS_DEBUG, "ACPI:    * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;

	if (intel_gma_init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	current += sizeof(igd_opregion_t);

	return acpi_align_current(current);
}

static const struct device_operations igd_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.write_acpi_tables = igd_write_opregion,
	.enable           = DEVICE_NOOP
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_1,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_2,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_3,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_4,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_1,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_2,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_3,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_4,
	0,
};

static const struct pci_driver integrated_graphics_driver __pci_driver = {
	.ops		= &igd_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
