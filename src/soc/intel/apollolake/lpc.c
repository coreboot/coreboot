/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/pci_ids.h>
#include <reg_script.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/lpc.h>
#include "chip.h"

static const struct reg_script lpc_serirq_enable[] = {
	/* Setup SERIRQ, enable continuous mode */
	REG_PCI_OR8(SERIRQ_CNTL, (1 << 7) | (1 << 6)),
#if !IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)
	REG_PCI_RMW8(SERIRQ_CNTL, ~(1 << 6), 0),
#endif
	REG_SCRIPT_END
};

static void enable_lpc_decode(struct device *lpc)
{
	const struct soc_intel_apollolake_config *config;

	if (!lpc || !lpc->chip_info)
		return;

	config = lpc->chip_info;

	/* Enable requested fixed IO decode ranges */
	pci_write_config16(lpc, LPC_EN, config->lpc_dec);

	/* Enable generic IO decode ranges */
	pci_write_config32(lpc, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(lpc, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(lpc, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(lpc, LPC_GEN4_DEC, config->gen4_dec);
}


static void lpc_init(struct device *dev)
{
	enable_lpc_decode(dev);
	reg_script_run_on_dev(dev, lpc_serirq_enable);
}


static void soc_lpc_add_io_resources(device_t dev)
{
	struct resource *res;

	/* Add the default claimed legacy IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_lpc_read_resources(device_t dev)
{
	/* Get the PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add IO resources to LPC. */
	soc_lpc_add_io_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources = &soc_lpc_read_resources,
	.set_resources = &pci_dev_set_resources,
	.enable_resources = &pci_dev_enable_resources,
	.write_acpi_tables = southbridge_write_acpi_tables,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.init = &lpc_init
};

static const struct pci_driver soc_lpc __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_APOLLOLAKE_LPC,
};
