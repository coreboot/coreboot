/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include "chip.h"

static const char *conn_acpi_name(const struct device *dev)
{
	static char name[5];
	snprintf(name, sizeof(name), "CON%1X", dev->path.generic.id);
	return name;
}

static const char *orientation_to_str(enum typec_orientation ori)
{
	switch (ori) {
	case TYPEC_ORIENTATION_NORMAL:
		return "normal";
	case TYPEC_ORIENTATION_REVERSE:
		return "reverse";
	case TYPEC_ORIENTATION_FOLLOW_CC: /* Intentional fallthrough */
	default:
		return "";
	}
}

static void conn_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_pmc_mux_conn_config *config = dev->chip_info;
	struct acpi_dp *dsd;
	const char *scope;
	const char *name;

	/* Reference the existing scope and write CONx device */
	scope = acpi_device_scope(dev);
	name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_name_integer("_ADR", dev->path.generic.id);

	/* _DSD, Device-Specific Data */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_integer(dsd, "usb2-port-number", config->usb2_port_number);
	acpi_dp_add_integer(dsd, "usb3-port-number", config->usb3_port_number);

	/*
	 * The kernel assumes that these Type-C signals (SBUs and HSLs) follow the CC lines,
	 * unless they are explicitly called out otherwise.
	 */
	if (config->sbu_orientation != TYPEC_ORIENTATION_FOLLOW_CC)
		acpi_dp_add_string(dsd, "sbu-orientation",
				   orientation_to_str(config->sbu_orientation));

	if (config->hsl_orientation != TYPEC_ORIENTATION_FOLLOW_CC)
		acpi_dp_add_string(dsd, "hsl-orientation",
				   orientation_to_str(config->hsl_orientation));

	acpi_dp_write(dsd);

	acpigen_pop_len(); /* CONx Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), dev->chip_ops->name,
	       dev_path(dev));
}

static struct device_operations conn_dev_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.acpi_name	= conn_acpi_name,
	.acpi_fill_ssdt	= conn_fill_ssdt,
};

static void conn_enable(struct device *dev)
{
	dev->ops = &conn_dev_ops;
}

struct chip_operations drivers_intel_pmc_mux_conn_ops = {
	CHIP_NAME("Intel PMC MUX CONN Driver")
	.enable_dev	= conn_enable,
};
