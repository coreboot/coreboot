/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>

#include "chip.h"

#define ACPI_DSM_PRIVACY_SCREEN_UUID	"C7033113-8720-4CEB-9090-9D52B3E52D73"

static void privacy_screen_detect_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_write_store();
	acpigen_emit_namestring(config->detect_function);
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_write_if_lequal_op_int(LOCAL2_OP, 1);
	acpigen_write_return_singleton_buffer(0xF);
	acpigen_pop_len();
}
static void privacy_screen_get_status_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(config->status_function);
}
static void privacy_screen_enable_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_namestring(config->enable_function);
}
static void privacy_screen_disable_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_namestring(config->disable_function);
}

static void (*privacy_screen_callbacks[])(void *) = {
	privacy_screen_detect_cb,
	privacy_screen_get_status_cb,
	privacy_screen_enable_cb,
	privacy_screen_disable_cb,
};

static void gfx_fill_ssdt_generator(struct device *dev)
{
	size_t i;
	struct drivers_gfx_generic_config *config = dev->chip_info;

	const char *scope = acpi_device_scope(dev);

	if (!scope)
		return;

	acpigen_write_scope(scope);

	/* Method (_DOD, 0) */
	acpigen_write_method("_DOD", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(config->device_count);
	for (i = 0; i < config->device_count; i++)
		acpigen_write_dword(config->device[i].addr);
	acpigen_pop_len(); /* End Package. */
	acpigen_pop_len(); /* End Method. */

	for (i = 0; i < config->device_count; i++) {
		acpigen_write_device(config->device[i].name);

		acpigen_write_name_integer("_ADR", config->device[i].addr);
		acpigen_write_name_integer("_STA", 0xF);

		if (config->device[i].privacy.enabled) {
			acpigen_write_dsm(ACPI_DSM_PRIVACY_SCREEN_UUID,
				privacy_screen_callbacks,
				ARRAY_SIZE(privacy_screen_callbacks),
				&config->device[i].privacy);
		}

		acpigen_pop_len(); /* Device */
	}
	acpigen_pop_len(); /* Scope */
}

static const char *gfx_acpi_name(const struct device *dev)
{
	struct drivers_gfx_generic_config *config = dev->chip_info;

	return config->name ? : "GFX0";
}

static struct device_operations gfx_ops = {
	.acpi_name		  = gfx_acpi_name,
	.acpi_fill_ssdt_generator = gfx_fill_ssdt_generator,
};

static void gfx_enable(struct device *dev)
{
	struct drivers_gfx_generic_config *config = dev->chip_info;

	if (!config)
		return;

	dev->ops = &gfx_ops;
}

struct chip_operations drivers_gfx_generic_ops = {
	CHIP_NAME("Generic Graphics Device")
	.enable_dev = gfx_enable
};
