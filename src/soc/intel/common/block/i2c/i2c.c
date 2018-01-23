/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
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
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <drivers/i2c/designware/dw_i2c.h>

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	int devfn;
	struct device *dev;
	struct resource *res;

	/* bus -> devfn */
	devfn = dw_i2c_soc_bus_to_devfn(bus);

	if (devfn < 0)
		return (uintptr_t)NULL;

	/* devfn -> dev */
	dev = dev_find_slot(0, devfn);
	if (!dev || !dev->enabled)
		return (uintptr_t)NULL;

	/* dev -> bar0 */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (res)
		return res->base;

	return (uintptr_t)NULL;
}

int dw_i2c_soc_dev_to_bus(struct device *dev)
{
	pci_devfn_t devfn = dev->path.pci.devfn;
	return dw_i2c_soc_devfn_to_bus(devfn);
}

static struct device_operations i2c_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_smbus,
	.ops_i2c_bus			= &dw_i2c_bus_ops,
	.ops_pci			= &pci_dev_ops_pci,
	.init				= &dw_i2c_dev_init,
	.acpi_fill_ssdt_generator	= &dw_i2c_acpi_fill_ssdt,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_I2C0,
	PCI_DEVICE_ID_INTEL_SPT_I2C1,
	PCI_DEVICE_ID_INTEL_SPT_I2C2,
	PCI_DEVICE_ID_INTEL_SPT_I2C3,
	PCI_DEVICE_ID_INTEL_SPT_I2C4,
	PCI_DEVICE_ID_INTEL_SPT_I2C5,
	PCI_DEVICE_ID_INTEL_KBP_H_I2C0,
	PCI_DEVICE_ID_INTEL_KBP_H_I2C1,
	PCI_DEVICE_ID_INTEL_KBP_H_I2C2,
	PCI_DEVICE_ID_INTEL_KBP_H_I2C3,
	PCI_DEVICE_ID_INTEL_APL_I2C0,
	PCI_DEVICE_ID_INTEL_APL_I2C1,
	PCI_DEVICE_ID_INTEL_APL_I2C2,
	PCI_DEVICE_ID_INTEL_APL_I2C3,
	PCI_DEVICE_ID_INTEL_APL_I2C4,
	PCI_DEVICE_ID_INTEL_APL_I2C5,
	PCI_DEVICE_ID_INTEL_APL_I2C6,
	PCI_DEVICE_ID_INTEL_APL_I2C7,
	PCI_DEVICE_ID_INTEL_CNL_I2C0,
	PCI_DEVICE_ID_INTEL_CNL_I2C1,
	PCI_DEVICE_ID_INTEL_CNL_I2C2,
	PCI_DEVICE_ID_INTEL_CNL_I2C3,
	PCI_DEVICE_ID_INTEL_CNL_I2C4,
	PCI_DEVICE_ID_INTEL_CNL_I2C5,
	PCI_DEVICE_ID_INTEL_GLK_I2C0,
	PCI_DEVICE_ID_INTEL_GLK_I2C1,
	PCI_DEVICE_ID_INTEL_GLK_I2C2,
	PCI_DEVICE_ID_INTEL_GLK_I2C3,
	PCI_DEVICE_ID_INTEL_GLK_I2C4,
	PCI_DEVICE_ID_INTEL_GLK_I2C5,
	PCI_DEVICE_ID_INTEL_GLK_I2C6,
	PCI_DEVICE_ID_INTEL_GLK_I2C7,
	0,
};

static const struct pci_driver pch_i2c __pci_driver = {
	.ops	 = &i2c_dev_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
