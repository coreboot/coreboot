/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>

#define GOOGLE_CHROMEEC_MUX_DEVICE_HID		"GOOG001A"
#define GOOGLE_CHROMEEC_MUX_DEVICE_NAME		"ECMX"

static void mux_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_scope(dev));
	acpigen_write_device(GOOGLE_CHROMEEC_MUX_DEVICE_NAME);
	acpigen_write_name_string("_HID", GOOGLE_CHROMEEC_MUX_DEVICE_HID);
	acpigen_write_name_string("_DDN", "ChromeOS EC Embedded Controller "
				  "Mux & Retimer control");

	acpigen_write_device_end();
	acpigen_write_scope_end();
}

static const char *mux_acpi_name(const struct device *dev)
{
	return GOOGLE_CHROMEEC_MUX_DEVICE_NAME;
}

static struct device_operations mux_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= mux_acpi_name,
	.acpi_fill_ssdt		= mux_fill_ssdt,
	.scan_bus		= scan_static_bus,
};

static void mux_enable(struct device *dev)
{
	dev->ops = &mux_ops;
}

struct chip_operations ec_google_chromeec_mux_ops = {
	CHIP_NAME("CrosEC Type C Mux device")
	.enable_dev = mux_enable
};
