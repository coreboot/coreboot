/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <device/device.h>
#include "chip.h"

#define TGL_PMC_MUX_HID "INTC105C"

static const char *mux_acpi_name(const struct device *dev)
{
	return "MUX";
}

static void mux_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	if (!scope || !name)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_name_string("_HID", TGL_PMC_MUX_HID);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static struct device_operations mux_dev_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.acpi_name	= mux_acpi_name,
	.acpi_fill_ssdt	= mux_fill_ssdt,
	.scan_bus	= scan_static_bus,
};

static void mux_enable(struct device *dev)
{
	dev->ops = &mux_dev_ops;
}

struct chip_operations drivers_intel_pmc_mux_ops = {
	CHIP_NAME("Intel PMC MUX Driver")
	.enable_dev	= mux_enable,
};
