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

#include <arch/acpigen.h>
#include <device/i2c.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <intelblocks/lpss_i2c.h>
#include "lpss_i2c.h"

static int lpss_i2c_dev_to_bus(struct device *dev)
{
	pci_devfn_t devfn = dev->path.pci.devfn;
	return i2c_soc_devfn_to_bus(devfn);
}

uintptr_t lpss_i2c_base_address(unsigned int bus)
{
	int devfn;
	struct device *dev;
	struct resource *res;

	/* bus -> devfn */
	devfn = i2c_soc_bus_to_devfn(bus);

	if (devfn < 0)
		return (uintptr_t)NULL;

	/* devfn -> dev */
	dev = dev_find_slot(0, devfn);
	if (!dev)
		return (uintptr_t)NULL;

	/* dev -> bar0 */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (res)
		return res->base;

	return (uintptr_t)NULL;
}

/*
 * Write ACPI object to describe speed configuration.
 *
 * ACPI Object: Name ("xxxx", Package () { scl_lcnt, scl_hcnt, sda_hold }
 *
 * SSCN: I2C_SPEED_STANDARD
 * FMCN: I2C_SPEED_FAST
 * FPCN: I2C_SPEED_FAST_PLUS
 * HSCN: I2C_SPEED_HIGH
 */
static void lpss_i2c_acpi_write_speed_config(
	const struct lpss_i2c_speed_config *config)
{
	if (!config)
		return;
	if (!config->scl_lcnt && !config->scl_hcnt && !config->sda_hold)
		return;

	if (config->speed >= I2C_SPEED_HIGH)
		acpigen_write_name("HSCN");
	else if (config->speed >= I2C_SPEED_FAST_PLUS)
		acpigen_write_name("FPCN");
	else if (config->speed >= I2C_SPEED_FAST)
		acpigen_write_name("FMCN");
	else
		acpigen_write_name("SSCN");

	/* Package () { scl_lcnt, scl_hcnt, sda_hold } */
	acpigen_write_package(3);
	acpigen_write_word(config->scl_hcnt);
	acpigen_write_word(config->scl_lcnt);
	acpigen_write_dword(config->sda_hold);
	acpigen_pop_len();
}

/*
 * The device should already be enabled and out of reset,
 * either from early init in coreboot or SiliconInit in FSP.
 */
static void lpss_i2c_dev_init(struct device *dev)
{
	const struct lpss_i2c_bus_config *config;
	int bus = lpss_i2c_dev_to_bus(dev);

	config = i2c_get_soc_cfg(bus, dev);

	if (!config || bus < 0)
		return;

	lpss_i2c_init(bus, config);
}

/*
 * Generate I2C timing information into the SSDT for the OS driver to consume,
 * optionally applying override values provided by the caller.
 */
static void lpss_i2c_acpi_fill_ssdt(struct device *dev)
{
	const struct lpss_i2c_bus_config *bcfg;
	struct lpss_i2c_regs *regs;
	struct lpss_i2c_speed_config sgen;
	enum i2c_speed speeds[LPSS_I2C_SPEED_CONFIG_COUNT] = {
		I2C_SPEED_STANDARD,
		I2C_SPEED_FAST,
		I2C_SPEED_FAST_PLUS,
		I2C_SPEED_HIGH,
	};
	int i, bus = lpss_i2c_dev_to_bus(dev);

	bcfg = i2c_get_soc_cfg(bus, dev);

	if (!bcfg)
		return;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs)
		return;

	acpigen_write_scope(acpi_device_path(dev));

	/* Report timing values for the OS driver */
	for (i = 0; i < LPSS_I2C_SPEED_CONFIG_COUNT; i++) {
		/* Generate speed config. */
		if (lpss_i2c_gen_speed_config(regs, speeds[i], bcfg, &sgen) < 0)
			continue;

		/* Generate ACPI based on selected speed config */
		lpss_i2c_acpi_write_speed_config(&sgen);
	}

	acpigen_pop_len();
}

static struct i2c_bus_operations i2c_bus_ops = {
	.dev_to_bus			= &lpss_i2c_dev_to_bus,
};

static struct device_operations i2c_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_smbus,
	.ops_i2c_bus			= &i2c_bus_ops,
	.init				= &lpss_i2c_dev_init,
	.acpi_fill_ssdt_generator	= &lpss_i2c_acpi_fill_ssdt,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_I2C0,
	PCI_DEVICE_ID_INTEL_SPT_I2C1,
	PCI_DEVICE_ID_INTEL_SPT_I2C2,
	PCI_DEVICE_ID_INTEL_SPT_I2C3,
	PCI_DEVICE_ID_INTEL_SPT_I2C4,
	PCI_DEVICE_ID_INTEL_SPT_I2C5,
	PCI_DEVICE_ID_INTEL_APL_I2C0,
	PCI_DEVICE_ID_INTEL_APL_I2C1,
	PCI_DEVICE_ID_INTEL_APL_I2C2,
	PCI_DEVICE_ID_INTEL_APL_I2C3,
	PCI_DEVICE_ID_INTEL_APL_I2C4,
	PCI_DEVICE_ID_INTEL_APL_I2C5,
	PCI_DEVICE_ID_INTEL_APL_I2C6,
	PCI_DEVICE_ID_INTEL_APL_I2C7,
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
