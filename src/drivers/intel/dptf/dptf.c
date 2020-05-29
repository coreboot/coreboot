/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include "chip.h"

static const char *dptf_acpi_name(const struct device *dev)
{
	return "DPTF";
}

/* Add custom tables and methods to SSDT */
static void dptf_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_dptf_config *config = dev->chip_info;

	printk(BIOS_INFO, "\\_SB.DPTF: %s at %s\n", dev->chip_ops->name, dev_path(dev));
}

/* Add static definitions of DPTF devices into the DSDT */
static void dptf_inject_dsdt(const struct device *dev)
{
	const struct drivers_intel_dptf_config *config;

	config = dev->chip_info;
	acpigen_write_scope("\\_SB");

	/* Toplevel DPTF device */
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name("_HID");
	acpigen_emit_eisaid("INT3400");
	acpigen_write_name_integer("_UID", 0);
	dptf_write_STA();

	acpigen_pop_len(); /* DPTF Device */
	acpigen_pop_len(); /* Scope */
}

static struct device_operations dptf_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= dptf_acpi_name,
	.acpi_fill_ssdt		= dptf_fill_ssdt,
	.acpi_inject_dsdt	= dptf_inject_dsdt,
};

static void dptf_enable_dev(struct device *dev)
{
	dev->ops = &dptf_ops;
}

struct chip_operations drivers_intel_dptf_ops = {
	CHIP_NAME("Intel DPTF")
	.enable_dev = dptf_enable_dev,
};
