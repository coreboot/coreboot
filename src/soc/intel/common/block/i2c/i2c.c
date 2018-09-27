/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-2018 Intel Corporation.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/chip.h>
#include <intelblocks/lpss.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

int dw_i2c_soc_dev_to_bus(struct device *dev)
{
	pci_devfn_t devfn = dev->path.pci.devfn;
	return dw_i2c_soc_devfn_to_bus(devfn);
}

/* Getting I2C bus configuration from devicetree config */
const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return &common_config->i2c[bus];
}

/* Get base address for early init of I2C controllers. */
uintptr_t dw_i2c_get_soc_early_base(unsigned int bus)
{
	return EARLY_I2C_BASE(bus);
}

#if !ENV_RAMSTAGE
static int lpss_i2c_early_init_bus(unsigned int bus)
{
	const struct dw_i2c_bus_config *config;
	const struct device *tree_dev;
	pci_devfn_t dev;
	int devfn;
	uintptr_t base;

	/* Find the PCI device for this bus controller */
	devfn = dw_i2c_soc_bus_to_devfn(bus);
	if (devfn < 0) {
		printk(BIOS_ERR, "I2C%u device not found\n", bus);
		return -1;
	}

	/* Look up the controller device in the devicetree */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));
	tree_dev = dev_find_slot(0, devfn);
	if (!tree_dev || !tree_dev->enabled) {
		printk(BIOS_ERR, "I2C%u device not enabled\n", bus);
		return -1;
	}

	/* Skip if not enabled for early init */
	config = dw_i2c_get_soc_cfg(bus);
	if (!config || !config->early_init) {
		printk(BIOS_DEBUG, "I2C%u not enabled for early init\n", bus);
		return -1;
	}

	/* Prepare early base address for access before memory */
	base = dw_i2c_get_soc_early_base(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config32(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	lpss_reset_release(base);

	/* Initialize the controller */
	if (dw_i2c_init(bus, config) < 0) {
		printk(BIOS_ERR, "I2C%u failed to initialize\n", bus);
		return -1;
	}

	return 0;
}

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	int devfn;
	pci_devfn_t dev;
	uintptr_t base;

	/* Find device+function for this controller */
	devfn = dw_i2c_soc_bus_to_devfn(bus);
	if (devfn < 0)
		return (uintptr_t)NULL;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);

	/* Attempt to initialize bus if base is not set yet */
	if (!base && !lpss_i2c_early_init_bus(bus))
		base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0),
				  16);
	return base;
}
#else

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

/*
 * This function ensures that the device is actually out of reset and
 * its ready for initialization sequence.
 */
static void dw_i2c_device_init(struct device *dev)
{
	uintptr_t base_address;
	int bus = dw_i2c_soc_dev_to_bus(dev);

	if (bus < 0)
		return;

	base_address = dw_i2c_base_address(bus);
	if (!base_address)
		return;

	/* Take device out of reset if its not done before */
	if (lpss_is_controller_in_reset(base_address))
		lpss_reset_release(base_address);

	dw_i2c_dev_init(dev);
}

static struct device_operations i2c_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_smbus,
	.ops_i2c_bus			= &dw_i2c_bus_ops,
	.ops_pci			= &pci_dev_ops_pci,
	.init				= &dw_i2c_device_init,
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
#endif
