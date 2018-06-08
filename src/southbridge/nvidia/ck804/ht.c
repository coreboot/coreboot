/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/acpi.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct device *dev;
	unsigned long mcfg_base;

	dev = dev_find_slot(0x0, PCI_DEVFN(0x0,0));
	if (!dev)
		return current;

	mcfg_base = pci_read_config16(dev, 0x90);
	if ((mcfg_base & 0x1000) == 0)
		return current;

	mcfg_base = (mcfg_base & 0xf) << 28;

	printk(BIOS_INFO, "mcfg_base %lx.\n", mcfg_base);

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)
			current, mcfg_base, 0x0, 0x0, 0xff);
	return current;
}

#endif

static void ht_init(struct device *dev)
{
	u32 htmsi;

	/* Enable HT MSI Mapping in capability register */
	htmsi = pci_read_config32(dev, 0xe0);
	htmsi |= (1 << 16);
	pci_write_config32(dev, 0xe0, htmsi);
}

static struct device_operations ht_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ht_init,
	.scan_bus         = 0,
	.ops_pci          = &ck804_pci_ops,
};

static const struct pci_driver ht_driver __pci_driver = {
	.ops    = &ht_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_HT,
};
