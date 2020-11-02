/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/path.h>
#include <device/soundwire.h>
#include <stdbool.h>

#include "soundwire.h"
#include "chip.h"

__weak int soc_fill_soundwire_controller(struct intel_soundwire_controller **controller)
{
	return -1;
}

static bool link_enabled(const struct device *dev, unsigned int link)
{
	struct device *child;

	for (child = dev->link_list->children; child; child = child->sibling) {
		if (child->enabled && child->path.type == DEVICE_PATH_GENERIC &&
		    child->path.generic.id == link)
			return true;
	}
	return false;
}

static void intel_soundwire_link_prop_cb(struct acpi_dp *dsd, unsigned int id,
					 const struct soundwire_controller *controller)
{
	struct intel_soundwire_controller *intel_controller =
		container_of(controller, struct intel_soundwire_controller, sdw);
	unsigned int quirk_mask = intel_controller->quirk_mask;

	/* Disable link if no are children enabled on this link device. */
	if (!link_enabled(intel_controller->dev, id))
		quirk_mask |= INTEL_SOUNDWIRE_QUIRK_BUS_DISABLE;

	acpi_dp_add_integer(dsd, "intel-sdw-ip-clock", intel_controller->ip_clock);
	acpi_dp_add_integer(dsd, "intel-quirk-mask", quirk_mask);
}

static void intel_soundwire_fill_ssdt(const struct device *dev)
{
	struct acpi_dp *dsd;
	struct intel_soundwire_controller *controller;
	const char *scope = acpi_device_scope(dev);

	if (!scope)
		return;

	if (soc_fill_soundwire_controller(&controller) < 0 || !controller)
		return;

	/* Provide device pointer for link property callback function. */
	controller->dev = dev;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_name_integer("_ADR", controller->acpi_address);
	acpigen_write_name_string("_CID", ACPI_HID_CONTAINER);

	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_controller(dsd, &controller->sdw, &intel_soundwire_link_prop_cb);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *intel_soundwire_acpi_name(const struct device *dev)
{
	return "SNDW";
}

static struct device_operations intel_soundwire_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= intel_soundwire_acpi_name,
	.acpi_fill_ssdt		= intel_soundwire_fill_ssdt,
	.scan_bus		= scan_static_bus,
};

static void intel_soundwire_enable(struct device *dev)
{
	dev->ops = &intel_soundwire_ops;
}

struct chip_operations drivers_intel_soundwire_ops = {
	CHIP_NAME("Intel SoundWire Controller")
	.enable_dev = intel_soundwire_enable
};
