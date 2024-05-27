/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpigen.h>
#include <stdio.h>
#include <string.h>

#include "h8.h"
#include "chip.h"

static char *h8_dsdt_scope(const struct device *dev, const char *scope)
{
	static char buf[DEVICE_PATH_MAX] = {};
	const char *path = acpi_device_path(dev);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf) - 1, "%s.%s", path, scope);

	return buf;
}

/*
 * Generates EC SSDT.
 */
void h8_ssdt_generator(const struct device *dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;

	if (!acpi_device_path(dev))
		return;

	printk(BIOS_INFO, "ACPI:    * H8\n");

	/* Scope HKEY */
	acpigen_write_scope(h8_dsdt_scope(dev, "HKEY"));

	/* Used by thinkpad_acpi */
	acpigen_write_name_byte("HBDC", h8_has_bdc(dev) ? ONE_OP : ZERO_OP);
	acpigen_write_name_byte("HWAN", h8_has_wwan(dev) ? ONE_OP : ZERO_OP);
	acpigen_write_name_byte("HKBL", (conf && conf->has_keyboard_backlight) ?
				ONE_OP : ZERO_OP);
	acpigen_write_name_byte("HUWB", (conf && conf->has_uwb) ?
				ONE_OP : ZERO_OP);

	acpigen_pop_len(); /* Scope HKEY */
}
