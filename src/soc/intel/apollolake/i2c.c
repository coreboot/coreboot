/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <soc/i2c.h>
#include <soc/intel/common/lpss_i2c.h>
#include <soc/pci_devs.h>
#include <soc/pci_ids.h>
#include "chip.h"

uintptr_t lpss_i2c_base_address(unsigned bus)
{
	unsigned devfn;
	struct device *dev;
	struct resource *res;

	/* bus -> devfn */
	devfn = i2c_bus_to_devfn(bus);
	if (devfn >= 0) {
		/* devfn -> dev */
		dev = dev_find_slot(0, devfn);
		if (dev) {
			/* dev -> bar0 */
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res)
				return res->base;
		}
	}

	return (uintptr_t)NULL;
}

static int i2c_dev_to_bus(struct device *dev)
{
	return i2c_devfn_to_bus(dev->path.pci.devfn);
}

/*
 * The device should already be enabled and out of reset,
 * either from early init in coreboot or FSP-S.
 */
static void i2c_dev_init(struct device *dev)
{
	struct soc_intel_apollolake_config *config = dev->chip_info;
	int bus = i2c_dev_to_bus(dev);

	if (!config || bus < 0)
		return;

	lpss_i2c_init(bus, &config->i2c[bus]);
}

static void i2c_fill_ssdt(struct device *dev)
{
	struct soc_intel_apollolake_config *config = dev->chip_info;
	int bus = i2c_dev_to_bus(dev);

	if (!config || bus < 0)
		return;

	acpigen_write_scope(acpi_device_path(dev));
	lpss_i2c_acpi_fill_ssdt(&config->i2c[bus]);
	acpigen_pop_len();
}

static struct i2c_bus_operations i2c_bus_ops = {
	.dev_to_bus			= &i2c_dev_to_bus,
};

static struct device_operations i2c_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_smbus,
	.ops_i2c_bus			= &i2c_bus_ops,
	.init				= &i2c_dev_init,
	.acpi_fill_ssdt_generator	= &i2c_fill_ssdt,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_APOLLOLAKE_I2C0,
	PCI_DEVICE_ID_APOLLOLAKE_I2C1,
	PCI_DEVICE_ID_APOLLOLAKE_I2C2,
	PCI_DEVICE_ID_APOLLOLAKE_I2C3,
	PCI_DEVICE_ID_APOLLOLAKE_I2C4,
	PCI_DEVICE_ID_APOLLOLAKE_I2C5,
	PCI_DEVICE_ID_APOLLOLAKE_I2C6,
	PCI_DEVICE_ID_APOLLOLAKE_I2C7,
	0,
};

static const struct pci_driver pch_i2c __pci_driver = {
	.ops	 = &i2c_dev_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
