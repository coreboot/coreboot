/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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

#include <console/console.h>
#include <arch/acpigen.h>
#include <string.h>

#include "h8.h"
#include "chip.h"

static char *h8_dsdt_scope(struct device *dev, const char *scope)
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
void h8_ssdt_generator(struct device *dev)
{
	if (!acpi_device_path(dev))
		return;

	printk(BIOS_INFO, "ACPI:    * H8\n");

	/* Scope HKEY */
	acpigen_write_scope(h8_dsdt_scope(dev, "HKEY"));

	/* Used by thinkpad_acpi */
	acpigen_write_name_byte("HBDC", h8_has_bdc(dev) ? ONE_OP : ZERO_OP);

	acpigen_pop_len(); /* Scope HKEY */
}
