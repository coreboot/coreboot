/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpi_pld.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_usb.h>

static const char *power_role_to_str(enum usb_typec_power_role power_role)
{
	switch (power_role) {
	case TYPEC_POWER_ROLE_SOURCE:
		return "source";
	case TYPEC_POWER_ROLE_SINK:
		return "sink";
	case TYPEC_POWER_ROLE_DUAL:
		return "dual";
	default:
		return "unknown";
	}
}

static const char *try_power_role_to_str(enum usb_typec_try_power_role try_power_role)
{
	switch (try_power_role) {
	case TYPEC_TRY_POWER_ROLE_NONE:
		/*
		 * This should never get returned; if there is no try-power role for a device,
		 * then the try-power-role field is not added to the DSD. Thus, this is just
		 * for completeness.
		 */
		return "none";
	case TYPEC_TRY_POWER_ROLE_SINK:
		return "sink";
	case TYPEC_TRY_POWER_ROLE_SOURCE:
		return "source";
	default:
		return "unknown";
	}
}

static const char *data_role_to_str(enum usb_typec_data_role data_role)
{
	switch (data_role) {
	case TYPEC_DATA_ROLE_DFP:
		return "host";
	case TYPEC_DATA_ROLE_UFP:
		return "device";
	case TYPEC_DATA_ROLE_DUAL:
		return "dual";
	default:
		return "unknown";
	}
}

/* Add port capabilities as DP properties */
static void add_port_caps(struct acpi_dp *dsd,
			  const struct typec_connector_class_config *config)
{
	acpi_dp_add_string(dsd, "power-role", power_role_to_str(config->power_role));
	acpi_dp_add_string(dsd, "data-role", data_role_to_str(config->data_role));

	if (config->try_power_role != TYPEC_TRY_POWER_ROLE_NONE)
		acpi_dp_add_string(dsd, "try-power-role",
				   try_power_role_to_str(config->try_power_role));
}

static void add_device_ref(struct acpi_dp *dsd,
			   const char *prop_name,
			   const struct device *dev)
{
	const char *path;
	char *fresh;

	if (!dev || !dev->enabled)
		return;

	/*
	 * Unfortunately, the acpi_dp_* API doesn't write out the data immediately, thus we need
	 * different storage areas for all of the strings, so strdup() is used for that. It is
	 * safe to use strdup() here, because the strings are generated at build-time and are
	 * guaranteed to be NUL-terminated (they come from the devicetree).
	 */
	path = acpi_device_path(dev);
	if (path) {
		fresh = strdup(path);
		if (fresh)
			acpi_dp_add_reference(dsd, prop_name, fresh);
	}
}

static void add_device_references(struct acpi_dp *dsd,
				  const struct typec_connector_class_config *config)
{
	/*
	 * Add references to the USB port objects so that the consumer of this information can
	 * know whether the port supports USB2, USB3, and/or USB4.
	 */
	add_device_ref(dsd, "usb2-port", config->usb2_port);
	add_device_ref(dsd, "usb3-port", config->usb3_port);
	add_device_ref(dsd, "usb4-port", config->usb4_port);

	/*
	 * Add references to the ACPI device(s) which control the orientation, USB data role and
	 * data muxing.
	 */
	add_device_ref(dsd, "orientation-switch", config->orientation_switch);
	add_device_ref(dsd, "usb-role-switch", config->usb_role_switch);
	add_device_ref(dsd, "mode-switch", config->mode_switch);
	add_device_ref(dsd, "retimer-switch", config->retimer_switch);
}

void acpigen_write_typec_connector(const struct typec_connector_class_config *config,
				   int port_number,
				   add_custom_dsd_property_cb add_custom_dsd_property)
{
	struct acpi_dp *dsd;
	char name[5];

	/* Create a CONx device */
	snprintf(name, sizeof(name), "CON%1X", port_number);
	acpigen_write_device(name);
	acpigen_write_name_integer("_ADR", port_number);

	dsd = acpi_dp_new_table("_DSD");

	/* Write out the _DSD table */
	acpi_dp_add_integer(dsd, "port-number", port_number);
	add_port_caps(dsd, config);
	add_device_references(dsd, config);

	/* Allow client to add custom properties if desired */
	if (add_custom_dsd_property)
		add_custom_dsd_property(dsd, port_number);
	acpi_dp_write(dsd);

	/* Add PLD */
	acpigen_write_pld(config->pld);

	acpigen_pop_len(); /* Device */
}
