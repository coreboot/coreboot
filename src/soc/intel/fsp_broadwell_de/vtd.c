/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/acpi.h>


static struct device_operations vtd_ops = {
	.write_acpi_tables        = vtd_write_acpi_tables,
};

static const struct pci_driver vtd_driver __pci_driver = {
	.ops    = &vtd_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = VTD_DEVID,
};
