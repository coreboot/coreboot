/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014-2019 Siemens AG
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
#include <device/pci_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/i2c.h>
#include <soc/iosf.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>

#include "chip.h"

/* Convert I2C bus number to PCI device and function */
int dw_i2c_soc_bus_to_devfn(unsigned int bus)
{
	if (bus <= 6)
		return PCI_DEVFN(SIO1_DEV, bus + 1);
	else
		return -1;
}

/* Convert PCI device and function to I2C bus number */
int dw_i2c_soc_dev_to_bus(struct device *dev)
{
	pci_devfn_t devfn = dev->path.pci.devfn;
	if ((devfn >= SOC_DEVFN_I2C1) && (devfn <= SOC_DEVFN_I2C7))
		return PCI_FUNC(devfn) - 1;
	else
		return -1;
}

/* Getting I2C bus configuration from devicetree config */
const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus)
{
	const struct soc_intel_fsp_baytrail_config *config;
	const struct device *dev = pcidev_path_on_root(SOC_DEVFN_SOC);

	if (dev && dev->chip_info) {
		config = dev->chip_info;
		return &config->i2c[bus];
	}

	die("Could not find SA_DEV_ROOT devicetree config!\n");
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
	tree_dev = pcidev_path_on_root(devfn);
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
	base = EARLY_I2C_BASE(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config32(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	write32((void *)((uint32_t)base + I2C_SOFTWARE_RESET), I2C_RESET_APB | I2C_RESET_FUNC);

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
		return 0;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	base = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & 0xfffffff0;

	/* Attempt to initialize bus if base is not set yet */
	if (!base && !lpss_i2c_early_init_bus(bus))
		base = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & 0xfffffff0;
	return base;
}
#else

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	int devfn;
	struct device *dev;
	struct resource *bar = NULL;

	/* bus -> devfn */
	devfn = dw_i2c_soc_bus_to_devfn(bus);

	if (devfn < 0)
		return (uintptr_t)NULL;

	/* devfn -> dev */
	dev = pcidev_path_on_root(devfn);
	if (dev && dev->enabled) {
		/* dev -> bar0 */
		bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	}

	if (bar)
		return bar->base;
	else
		return (uintptr_t)NULL;
}

static void i2c_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index)
{
	struct resource *bar;
	global_nvs_t *gnvs;
	uint32_t val;

	/* Find ACPI NVS to update BARs */
	gnvs = (global_nvs_t *)cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to locate Global NVS\n");
		return;
	}

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		gnvs->dev.lpss_bar0[nvs_index] = (uint32_t)bar->base;

	bar = find_resource(dev, PCI_BASE_ADDRESS_1);
	if (bar)
		gnvs->dev.lpss_bar1[nvs_index] = (uint32_t)bar->base;

	/* Device is enabled in ACPI mode */
	gnvs->dev.lpss_en[nvs_index] = 1;

	/* Put device in ACPI mode */
	val = iosf_lpss_read(iosf_reg);
	val |= (LPSS_CTL_PCI_CFG_DIS | LPSS_CTL_ACPI_INT_EN);
	iosf_lpss_write(iosf_reg, val);
	val = pci_read_config32(dev, PCI_COMMAND);
	val |= PCI_COMMAND_INT_DISABLE;
	pci_write_config32(dev, PCI_COMMAND, val);
}

static void dev_enable_snoop_and_pm(struct device *dev, int iosf_reg)
{
	uint32_t val;

	val = iosf_lpss_read(iosf_reg);
	val &= ~(LPSS_CTL_SNOOP | LPSS_CTL_NOSNOOP);
	val |= (LPSS_CTL_SNOOP | LPSS_CTL_PM_CAP_PRSNT);
	iosf_lpss_write(iosf_reg, val);
}

static void dev_ctl_reg(struct device *dev, int *iosf_reg, int *nvs_index)
{
	int bus;

	bus = dw_i2c_soc_dev_to_bus(dev);
	if (bus >= 0) {
		*iosf_reg = LPSS_I2C1_CTL + (bus * 8);
		*nvs_index = bus + 1;
	} else {

		*iosf_reg = -1;
		*nvs_index = -1;
	}
}

static void i2c_disable_resets(struct device *dev)
{
	uint32_t base;

	printk(BIOS_DEBUG, "Releasing I2C device from reset.\n");
	base = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & 0xfffffff0;
	write32((void *)(base + I2C_SOFTWARE_RESET), I2C_RESET_APB | I2C_RESET_FUNC);
}

static void i2c_lpss_init(struct device *dev)
{
	struct soc_intel_fsp_baytrail_config *config = dev->chip_info;
	int iosf_reg, nvs_index;

	dev_ctl_reg(dev, &iosf_reg, &nvs_index);

	if (iosf_reg < 0) {
		int slot = PCI_SLOT(dev->path.pci.devfn);
		int func = PCI_FUNC(dev->path.pci.devfn);
		printk(BIOS_DEBUG, "Could not find iosf_reg for %02x.%01x\n",
		       slot, func);
		return;
	}
	dev_enable_snoop_and_pm(dev, iosf_reg);
	i2c_disable_resets(dev);

	if (config && (config->PcdLpssSioEnablePciMode == LPSS_PCI_MODE_DISABLE))
		i2c_enable_acpi_mode(dev, iosf_reg, nvs_index);
}
/*
 * This function ensures that the device is actually out of reset and
 * it is ready for initialization sequence.
 */
static void dw_i2c_device_init(struct device *dev)
{
	int bus = dw_i2c_soc_dev_to_bus(dev);

	if (bus < 0)
		return;

	if (!dw_i2c_base_address(bus))
		return;
	i2c_lpss_init(dev);
	dw_i2c_dev_init(dev);
}

static struct device_operations i2c_dev_ops = {
	.read_resources			= pci_dev_read_resources,
	.set_resources			= pci_dev_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.scan_bus			= scan_smbus,
	.ops_i2c_bus			= &dw_i2c_bus_ops,
	.ops_pci			= &pci_dev_ops_pci,
	.init				= dw_i2c_device_init,
	.acpi_fill_ssdt_generator	= dw_i2c_acpi_fill_ssdt,
};

static const unsigned short pci_device_ids[] = {
	I2C1_DEVID,
	I2C2_DEVID,
	I2C3_DEVID,
	I2C4_DEVID,
	I2C5_DEVID,
	I2C6_DEVID,
	I2C7_DEVID,
	0
};

static const struct pci_driver pch_i2c __pci_driver = {
	.ops	 = &i2c_dev_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
#endif
