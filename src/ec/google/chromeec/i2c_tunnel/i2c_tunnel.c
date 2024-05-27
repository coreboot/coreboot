/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdio.h>

#include "chip.h"

#define CROS_EC_I2C_TUNNEL_HID		"GOOG0012"
#define CROS_EC_I2C_TUNNEL_DDN		"Cros EC I2C Tunnel"

static void crosec_i2c_tunnel_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	struct ec_google_chromeec_i2c_tunnel_config *cfg = dev->chip_info;
	struct acpi_dp *dsd;

	if (!scope || !cfg)
		return;

	acpigen_write_scope(scope);

	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", CROS_EC_I2C_TUNNEL_HID);
	acpigen_write_name_integer("_UID", cfg->uid);
	acpigen_write_name_string("_DDN", CROS_EC_I2C_TUNNEL_DDN);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_integer(dsd, "google,remote-bus", cfg->remote_bus);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), CROS_EC_I2C_TUNNEL_DDN,
						dev_path(dev));
}

static const char *crosec_i2c_tunnel_acpi_name(const struct device *dev)
{
	struct ec_google_chromeec_i2c_tunnel_config *cfg = dev->chip_info;
	static char name[5];

	if (cfg->name)
		return cfg->name;

	snprintf(name, sizeof(name), "TUN%X", dev->path.generic.id);
	return name;
}

static struct device_operations crosec_i2c_tunnel_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= crosec_i2c_tunnel_acpi_name,
	.acpi_fill_ssdt		= crosec_i2c_tunnel_fill_ssdt,
	.scan_bus		= scan_static_bus,
};

static void crosec_i2c_tunnel_enable(struct device *dev)
{
	dev->ops = &crosec_i2c_tunnel_ops;
}

struct chip_operations ec_google_chromeec_i2c_tunnel_ops = {
	.name = "CrosEC I2C Tunnel Device",
	.enable_dev = crosec_i2c_tunnel_enable
};
