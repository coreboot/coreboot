/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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


#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/iosf.h>
#include <soc/nvs.h>
#include <soc/ramstage.h>

void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index)
{
	struct resource *bar;
	global_nvs_t *gnvs;

	printk(BIOS_SPEW, "%s/%s ( %s, 0x%08x, 0x%08x )\n",
			__FILE__, __func__, dev_name(dev), iosf_reg, nvs_index);

	/* Find ACPI NVS to update BARs */
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to locate Global NVS\n");
		return;
	}

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		gnvs->dev.scc_bar0[nvs_index] = bar->base;
	bar = find_resource(dev, PCI_BASE_ADDRESS_2);
	if (bar)
		gnvs->dev.scc_bar1[nvs_index] = bar->base;

	/* Device is enabled in ACPI mode */
	gnvs->dev.scc_en[nvs_index] = 1;
}
