/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <assert.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip.h"

#define TZ_DEVICE_PATH	"\\_TZ"
/* These defaults should be good enough for most systems */
#define DEFAULT_TC1	2
#define DEFAULT_TC2	5
#define DEFAULT_TSP	10

#define CELSIUS_TO_DECI_KELVIN(temp_c)	((temp_c) * 10 + 2732)
#define SECONDS_TO_DECI_SECONDS(s)	((s) * 10)

static const char *thermal_zone_acpi_name(const struct device *dev)
{
	char *name;

	if (dev->path.type != DEVICE_PATH_GENERIC)
		return NULL;

	name = malloc(ACPI_NAME_BUFFER_SIZE);
	snprintf(name, ACPI_NAME_BUFFER_SIZE, "TM%02X", dev->path.generic.id);

	return name;
}

static void thermal_zone_fill_ssdt(const struct device *dev)
{
	struct drivers_acpi_thermal_zone_config *config = config_of(dev);
	const char *scope;
	const char *name;

	assert(dev->path.type == DEVICE_PATH_GENERIC);

	if (config->use_acpi1_thermal_zone_scope)
		scope = TZ_DEVICE_PATH;
	else
		scope = acpi_device_scope(dev);

	name = acpi_device_name(dev);

	assert(name);
	assert(scope);

	if (!config->temperature_controller) {
		printk(BIOS_ERR, "%s: missing temperature_controller\n", dev_path(dev));
		return;
	}

	printk(BIOS_INFO, "%s.%s: %s at %s\n", scope, name, dev->chip_ops->name, dev_path(dev));

	acpigen_write_scope(scope);
	acpigen_write_thermal_zone(name);

	if (config->description)
		acpigen_write_name_unicode("_STR", config->description);

	if (config->polling_period)
		acpigen_write_name_integer(
			"_TZP", SECONDS_TO_DECI_SECONDS(config->polling_period));

	if (config->critical_temperature)
		acpigen_write_name_integer(
			"_CRT", CELSIUS_TO_DECI_KELVIN(config->critical_temperature));

	if (config->hibernate_temperature)
		acpigen_write_name_integer(
			"_HOT", CELSIUS_TO_DECI_KELVIN(config->hibernate_temperature));

	if (config->passive_config.temperature) {
		acpigen_write_name_integer(
			"_PSV", CELSIUS_TO_DECI_KELVIN(config->passive_config.temperature));

		/*
		 * The linux kernel currently has an artificial limit of 10 on the number of
		 * references that can be returned in a list. If we don't respect this limit,
		 * then the passive threshold won't work.
		 *
		 * See https://source.chromium.org/chromiumos/chromiumos/codesearch/+/main:src/third_party/kernel/v5.10/include/acpi/acpi_bus.h;l=19
		 */
		acpigen_write_processor_package("_PSL", 0, MIN(10, dev_count_cpu()));

		acpigen_write_name_integer("_TC1", config->passive_config.time_constant_1
							   ?: DEFAULT_TC1);
		acpigen_write_name_integer("_TC2", config->passive_config.time_constant_2
							   ?: DEFAULT_TC2);
		acpigen_write_name_integer(
			"_TSP",
			SECONDS_TO_DECI_SECONDS(config->passive_config.time_sampling_period
							?: DEFAULT_TSP));
	}

	/*
	 * Method (_TMP) {
	 *   Return (<path>.TMP(<sensor_id>))
	 * }
	 */
	acpigen_write_method_serialized("_TMP", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(acpi_device_path_join(config->temperature_controller, "TMP"));
	acpigen_write_integer(config->sensor_id);
	acpigen_write_method_end();

	acpigen_write_thermal_zone_end();
	acpigen_write_scope_end();
}

static struct device_operations thermal_zone_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= thermal_zone_acpi_name,
	.acpi_fill_ssdt		= thermal_zone_fill_ssdt,
};

static void thermal_zone_enable_dev(struct device *dev)
{
	dev->ops = &thermal_zone_ops;
}

struct chip_operations drivers_acpi_thermal_zone_ops = {
	.name = "ACPI Thermal Zone",
	.enable_dev = thermal_zone_enable_dev,
};
