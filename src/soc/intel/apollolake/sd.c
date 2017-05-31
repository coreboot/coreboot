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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static void sd_fill_ssdt(struct device *dev)
{
	config_t *config = dev->chip_info;
	const char *path;
	struct acpi_gpio default_gpio = {
		.type = ACPI_GPIO_TYPE_INTERRUPT,
		.pull = ACPI_GPIO_PULL_NONE,
		.irq.mode = ACPI_IRQ_EDGE_TRIGGERED,
		.irq.polarity = ACPI_IRQ_ACTIVE_BOTH,
		.irq.shared = ACPI_IRQ_SHARED,
		.irq.wake = ACPI_IRQ_WAKE,
		.interrupt_debounce_timeout = 10000, /* 100ms */
		.pin_count = 1,
		.pins = { config->sdcard_cd_gpio }
	};
	struct acpi_dp *dp;

	if (!dev->enabled)
		return;

	/* Use device path as the Scope for the SSDT */
	path = acpi_device_path(dev);
	if (!path)
		return;
	if (!config->sdcard_cd_gpio)
		return;
	acpigen_write_scope(path);
	acpigen_write_name("_CRS");

	/* Write GpioInt() as default (if set) or custom from devicetree */
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(&default_gpio);
	acpigen_write_resourcetemplate_footer();

	/* Bind the cd-gpio name to the GpioInt() resource */
	dp = acpi_dp_new_table("_DSD");
	acpi_dp_add_gpio(dp, "cd-gpio", path, 0, 0, 1);
	acpi_dp_write(dp);

	acpigen_pop_len();
}
#endif

static struct device_operations dev_ops = {
	.read_resources		  = &pci_dev_read_resources,
	.set_resources		  = &pci_dev_set_resources,
	.enable_resources	  = &pci_dev_enable_resources,
	.ops_pci		  = &soc_pci_ops,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = &sd_fill_ssdt,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_SD,
	PCI_DEVICE_ID_INTEL_GLK_SD,
	0,
};

static const struct pci_driver pch_sd __pci_driver = {
	.ops	= &dev_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices= pci_device_ids,
};
