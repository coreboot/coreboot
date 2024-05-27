/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_pld.h>
#include <acpi/acpigen.h>
#include <boot/coreboot_tables.h>
#include <cbmem.h>
#include <console/console.h>
#include <drivers/usb/acpi/chip.h>
#include <intelblocks/acpi.h>
#include <stdio.h>

#include "chip.h"

/* Number of registered connectors */
static size_t total_conn_count;

static void conn_init(struct device *dev)
{
	total_conn_count++;
}

static unsigned int get_usb_port_number(const struct device *usb_port)
{
	return usb_port->path.usb.port_id + 1;
}

static struct type_c_info *conn_get_cbmem_buffer(void)
{
	struct type_c_info *info;
	size_t size;

	info = cbmem_find(CBMEM_ID_TYPE_C_INFO);
	if (info)
		return info;

	size = sizeof(struct type_c_info) + total_conn_count * sizeof(struct type_c_port_info);
	info = cbmem_add(CBMEM_ID_TYPE_C_INFO, size);

	if (!info)
		return NULL;

	memset(info, 0, size);
	return info;
}

static void conn_write_cbmem_entry(struct device *dev)
{
	const struct drivers_intel_pmc_mux_conn_config *config = dev->chip_info;
	struct type_c_port_info *port_info;
	struct type_c_info *info;
	size_t count;

	/*
	 * Do not re-run this code on resume as the cbmem data is populated on boot-up
	 * (non-S3 path) and stays intact across S3 suspend/resume.
	 */
	if (acpi_is_wakeup_s3())
		return;

	info = conn_get_cbmem_buffer();
	if (!info || (info->port_count >= total_conn_count)) {
		printk(BIOS_ERR, "No space for Type-C port info!\n");
		return;
	}

	count = info->port_count;
	port_info = &info->port_info[count];
	port_info->usb2_port_number = get_usb_port_number(config->usb2_port);
	port_info->usb3_port_number = get_usb_port_number(config->usb3_port);
	port_info->sbu_orientation = config->sbu_orientation;
	port_info->data_orientation = config->hsl_orientation;

	printk(BIOS_INFO, "added type-c port%zu info to cbmem: usb2:%d usb3:%d sbu:%d data:%d\n",
			count, port_info->usb2_port_number, port_info->usb3_port_number,
			port_info->sbu_orientation, port_info->data_orientation);

	info->port_count++;
}

static const char *conn_acpi_name(const struct device *dev)
{
	static char name[5];
	snprintf(name, sizeof(name), "CON%1X", dev->path.generic.id);
	return name;
}

static const char *orientation_to_str(enum type_c_orientation ori)
{
	switch (ori) {
	case TYPEC_ORIENTATION_NORMAL:
		return "normal";
	case TYPEC_ORIENTATION_REVERSE:
		return "reverse";
	case TYPEC_ORIENTATION_NONE: /* Intentional fallthrough */
	default:
		return "";
	}
}

static void get_pld_from_usb_ports(struct acpi_pld *pld,
	struct device *usb2_port, struct device *usb3_port)
{
	struct drivers_usb_acpi_config *config = NULL;

	if (usb3_port)
		config = usb3_port->chip_info;
	else if (usb2_port)
		config = usb2_port->chip_info;

	if (config) {
		if (config->use_custom_pld)
			*pld = config->custom_pld;
		else
			acpi_pld_fill_usb(pld, config->type, &config->group);
	}
}

static void conn_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_pmc_mux_conn_config *config = dev->chip_info;
	struct acpi_dp *dsd;
	const char *scope;
	const char *name;
	struct acpi_pld pld = {0};

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
	acpi_dp_add_integer(dsd, "usb2-port-number", get_usb_port_number(config->usb2_port));
	acpi_dp_add_integer(dsd, "usb3-port-number", get_usb_port_number(config->usb3_port));

	/*
	 * The kernel assumes that these Type-C signals (SBUs and HSLs) follow the CC lines,
	 * unless they are explicitly called out otherwise.
	 */
	if (config->sbu_orientation != TYPEC_ORIENTATION_NONE)
		acpi_dp_add_string(dsd, "sbu-orientation",
				   orientation_to_str(config->sbu_orientation));

	if (config->hsl_orientation != TYPEC_ORIENTATION_NONE)
		acpi_dp_add_string(dsd, "hsl-orientation",
				   orientation_to_str(config->hsl_orientation));

	acpi_dp_write(dsd);

	/* Copy _PLD from USB ports */
	get_pld_from_usb_ports(&pld, config->usb2_port, config->usb3_port);
	acpigen_write_pld(&pld);

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
	.init		= conn_init,
	.final		= conn_write_cbmem_entry,
};

static void conn_enable(struct device *dev)
{
	dev->ops = &conn_dev_ops;
}

struct chip_operations drivers_intel_pmc_mux_conn_ops = {
	.name = "Intel PMC MUX CONN Driver",
	.enable_dev	= conn_enable,
};

bool intel_pmc_mux_conn_get_ports(const struct device *conn, unsigned int *usb2_port,
					unsigned int *usb3_port)
{
	const struct drivers_intel_pmc_mux_conn_config *mux_config;

	if (!conn->chip_info || conn->chip_ops != &drivers_intel_pmc_mux_conn_ops)
		return false;

	mux_config = conn->chip_info;
	*usb2_port = get_usb_port_number(mux_config->usb2_port);
	*usb3_port = get_usb_port_number(mux_config->usb3_port);

	return true;
};
