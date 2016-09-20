/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/iomap.h>
#include <soc/pci_ids.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <timer.h>
#include "chip.h"

/*
 * The ACPI IO BAR (offset 0x20) is not PCI compliant. We've observed cases
 * where the BAR reads back as 0, but the IO window is open. This also means
 * that it will not respond to PCI probing. In the event that probing the BAR
 * fails, we still need to create a resource for it.
 */
static void read_resources(device_t dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = ACPI_PMIO_BASE;
	res->size = ACPI_PMIO_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

/*
 * Part 2:
 * Resources are assigned, and no other device was given an IO resource to
 * overlap with our ACPI BAR. But because the resource is FIXED,
 * pci_dev_set_resources() will not store it for us. We need to do that
 * explicitly.
 */
static void set_resources(device_t dev)
{
	struct resource *res;

	pci_dev_set_resources(dev);

	res = find_resource(dev, PCI_BASE_ADDRESS_4);
	pci_write_config32(dev, res->index, res->base);
	dev->command |= PCI_COMMAND_IO;
	res->flags |= IORESOURCE_STORED;
	report_resource_stored(dev, res, " ACPI BAR");
}

static void pmc_gpe_init(void)
{
	uint32_t gpio_cfg = 0;
	uint32_t gpio_cfg_reg;
	uint8_t dw1, dw2, dw3;
	const struct soc_intel_apollolake_config *config;

	struct device *dev = NB_DEV_ROOT;
	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}
	config = dev->chip_info;

	uintptr_t pmc_bar = get_pmc_mmio_bar();

	const uint32_t gpio_cfg_mask =
		(GPE0_DWX_MASK << GPE0_DW1_SHIFT) |
		(GPE0_DWX_MASK << GPE0_DW2_SHIFT) |
		(GPE0_DWX_MASK << GPE0_DW3_SHIFT);

	/* Assign to local variable */
	dw1 = config->gpe0_dw1;
	dw2 = config->gpe0_dw2;
	dw3 = config->gpe0_dw3;

	/* Making sure that bad values don't bleed into the other fields */
	dw1 &= GPE0_DWX_MASK;
	dw2 &= GPE0_DWX_MASK;
	dw3 &= GPE0_DWX_MASK;

	/* Route the GPIOs to the GPE0 block. Determine that all values
	 * are different, and if they aren't use the reset values.
	 * DW0 is reserved/unused */
	if (dw1 == dw2 || dw2 == dw3) {
		printk(BIOS_INFO, "PMC: Using default GPE route.\n");
		gpio_cfg = read32((void *)pmc_bar + GPIO_GPE_CFG);

		dw1 = (gpio_cfg >> GPE0_DW1_SHIFT) & GPE0_DWX_MASK;
		dw2 = (gpio_cfg >> GPE0_DW2_SHIFT) & GPE0_DWX_MASK;
		dw3 = (gpio_cfg >> GPE0_DW3_SHIFT) & GPE0_DWX_MASK;
	} else {
		gpio_cfg |= (uint32_t)dw1 << GPE0_DW1_SHIFT;
		gpio_cfg |= (uint32_t)dw2 << GPE0_DW2_SHIFT;
		gpio_cfg |= (uint32_t)dw3 << GPE0_DW3_SHIFT;
	}

	gpio_cfg_reg = read32((void *)pmc_bar + GPIO_GPE_CFG) & ~gpio_cfg_mask;
	gpio_cfg_reg |= gpio_cfg & gpio_cfg_mask;

	write32((void *)pmc_bar + GPIO_GPE_CFG, gpio_cfg_reg);

	/* Set the routes in the GPIO communities as well. */
	gpio_route_gpe(dw1, dw2, dw3);

	/* Reset the power state in cbmem as routing */
	fixup_power_state();
}

static void pch_set_acpi_mode(void)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER) && !acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT);
		printk(BIOS_DEBUG, "Done.\n");
	}
}

static int choose_slp_s3_assertion_width(int width_usecs)
{
	int i;
	static const struct {
		int max_width;
		int value;
	} slp_s3_settings[] = {
		{
			.max_width = 60,
			.value = SLP_S3_ASSERT_60_USEC,
		},
		{
			.max_width = 1 * USECS_PER_MSEC,
			.value = SLP_S3_ASSERT_1_MSEC,
		},
		{
			.max_width = 50 * USECS_PER_MSEC,
			.value = SLP_S3_ASSERT_50_MSEC,
		},
		{
			.max_width =  2 * USECS_PER_SEC,
			.value = SLP_S3_ASSERT_2_SEC,
		},
	};

	for (i = 0; i < ARRAY_SIZE(slp_s3_settings); i++) {
		if (width_usecs <= slp_s3_settings[i].max_width)
			break;
	}

	/* Provide conservative default if nothing set in devicetree
	 * or requested assertion width too large. */
	if (width_usecs <= 0 || i == ARRAY_SIZE(slp_s3_settings))
		i = ARRAY_SIZE(slp_s3_settings) - 1;

	printk(BIOS_DEBUG, "SLP S3 assertion width: %d usecs\n",
		slp_s3_settings[i].max_width);

	return slp_s3_settings[i].value;
}

static void set_slp_s3_assertion_width(int width_usecs)
{
	uint32_t reg;
	uintptr_t gen_pmcon3 = get_pmc_mmio_bar() + GEN_PMCON3;
	int setting = choose_slp_s3_assertion_width(width_usecs);

	reg = read32((void *)gen_pmcon3);
	reg &= ~SLP_S3_ASSERT_MASK;
	reg |= setting << SLP_S3_ASSERT_WIDTH_SHIFT;
	write32((void *)gen_pmcon3, reg);
}

static void pmc_init(struct device *dev)
{
	const struct soc_intel_apollolake_config *cfg = dev->chip_info;

	/* Set up GPE configuration */
	pmc_gpe_init();
	pch_set_acpi_mode();

	if (cfg != NULL)
		set_slp_s3_assertion_width(cfg->slp_s3_assertion_width_usecs);

	/* Log power state */
	pch_log_state();
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init                   = &pmc_init,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_APOLLOLAKE_PMC,
};
