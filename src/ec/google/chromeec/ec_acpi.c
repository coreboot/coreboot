/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <arch/acpi.h>
#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <drivers/usb/acpi/chip.h>
#include <stdlib.h>

#include "chip.h"
#include "ec.h"
#include "ec_commands.h"

#define GOOGLE_CHROMEEC_USBC_DEVICE_HID  "GOOG0014"
#define GOOGLE_CHROMEEC_USBC_DEVICE_NAME "USBC"

const char *google_chromeec_acpi_name(const struct device *dev)
{
	return "EC0";
}

static const char *power_role_to_str(enum ec_pd_power_role_caps power_role)
{
	switch (power_role) {
	case EC_PD_POWER_ROLE_SOURCE:
		return "source";
	case EC_PD_POWER_ROLE_SINK:
		return "sink";
	case EC_PD_POWER_ROLE_DUAL:
		return "dual";
	default:
		return "unknown";
	}
}

static const char *try_power_role_to_str(enum ec_pd_try_power_role_caps try_power_role)
{
	switch (try_power_role) {
	case EC_PD_TRY_POWER_ROLE_NONE:
		/*
		 * This should never get returned; if there is no try-power role for a device,
		 * then the try-power-role field is not added to the DSD. Thus, this is just
		 * for completeness.
		 */
		return "none";
	case EC_PD_TRY_POWER_ROLE_SINK:
		return "sink";
	case EC_PD_TRY_POWER_ROLE_SOURCE:
		return "source";
	default:
		return "unknown";
	}
}

static const char *data_role_to_str(enum ec_pd_data_role_caps data_role)
{
	switch (data_role) {
	case EC_PD_DATA_ROLE_DFP:
		return "host";
	case EC_PD_DATA_ROLE_UFP:
		return "device";
	case EC_PD_DATA_ROLE_DUAL:
		return "dual";
	default:
		return "unknown";
	}
}

/*
 * Apparently these are supposed to be uppercase, in contrast to the other
 * lowercase fields.
 */
static const char *port_location_to_str(enum ec_pd_port_location port_location)
{
	switch (port_location) {
	case EC_PD_PORT_LOCATION_LEFT:
		return "LEFT";
	case EC_PD_PORT_LOCATION_RIGHT:
		return "RIGHT";
	case EC_PD_PORT_LOCATION_BACK:
		return "BACK";
	case EC_PD_PORT_LOCATION_FRONT:
		return "FRONT";
	case EC_PD_PORT_LOCATION_LEFT_FRONT:
		return "LEFT_FRONT";
	case EC_PD_PORT_LOCATION_LEFT_BACK:
		return "LEFT_BACK";
	case EC_PD_PORT_LOCATION_RIGHT_FRONT:
		return "RIGHT_FRONT";
	case EC_PD_PORT_LOCATION_RIGHT_BACK:
		return "RIGHT_BACK";
	case EC_PD_PORT_LOCATION_BACK_LEFT:
		return "BACK_LEFT";
	case EC_PD_PORT_LOCATION_BACK_RIGHT:
		return "BACK_RIGHT";
	case EC_PD_PORT_LOCATION_UNKNOWN: /* intentional fallthrough */
	default:
		return "UNKNOWN";
	}
}

/* Add port capabilities as DP properties */
static void add_port_caps(struct acpi_dp *dsd, const struct usb_pd_port_caps *port_caps)
{
	acpi_dp_add_string(dsd, "power-role", power_role_to_str(port_caps->power_role_cap));

	if (port_caps->try_power_role_cap != EC_PD_TRY_POWER_ROLE_NONE)
		acpi_dp_add_string(dsd, "try-power-role",
			try_power_role_to_str(port_caps->try_power_role_cap));

	acpi_dp_add_string(dsd, "data-role", data_role_to_str(port_caps->data_role_cap));
	acpi_dp_add_string(dsd, "port-location", port_location_to_str(
				port_caps->port_location));
}

/*
 * Helper for fill_ssdt_generator. This adds references to the USB
 * port objects so that the consumer of this information can know
 * whether the port supports USB2 and/or USB3.
 */
static void add_usb_port_references(struct acpi_dp *dsd, int port_number)
{
	static const char usb2_port[] = "usb2-port";
	static const char usb3_port[] = "usb3-port";
	struct device *port = NULL;
	const char *path;
	const char *usb_port_type;
	struct drivers_usb_acpi_config *config;

	/*
	 * Unfortunately, the acpi_dp_* API doesn't write out the data immediately, thus we need
	 * different storage areas for all of the strings, so strdup() is used for that. It is
	 * safe to use strdup() here, because the strings are generated at build-time and are
	 * guaranteed to be NUL-terminated (they come from the devicetree).
	 */
	while ((port = dev_find_path(port, DEVICE_PATH_USB)) != NULL) {
		if (!port->enabled || port->path.type != DEVICE_PATH_USB)
			continue;

		/* Looking for USB 2 & 3 port devices only */
		if (port->path.usb.port_type == 2)
			usb_port_type = usb2_port;
		else if (port->path.usb.port_type == 3)
			usb_port_type = usb3_port;
		else
			continue;

		config = port->chip_info;

		/*
		 * Look at only USB Type-C ports, making sure they match the
		 * port number we're looking for (the 'token' field in 'group').
		 * Also note that 'port_number' is 0-based, whereas the 'token'
		 * field is 1-based.
		 */
		if ((config->type != UPC_TYPE_C_USB2_ONLY) &&
		    (config->type != UPC_TYPE_C_USB2_SS_SWITCH) &&
		    (config->type != UPC_TYPE_C_USB2_SS))
			continue;

		if (config->group.token != (port_number + 1))
			continue;

		path = acpi_device_path(port);
		if (path) {
			path = strdup(path);
			if (!path)
				continue;

			acpi_dp_add_reference(dsd, usb_port_type, path);
		}
	}
}

static void fill_ssdt_typec_device(int num_ports)
{
	struct usb_pd_port_caps port_caps;
	char con_name[] = "CONx";
	struct acpi_dp *dsd;
	int rv;
	int i;

	acpigen_write_device(GOOGLE_CHROMEEC_USBC_DEVICE_NAME);
	acpigen_write_name_string("_HID", GOOGLE_CHROMEEC_USBC_DEVICE_HID);
	acpigen_write_name_string("_DDN", "ChromeOS EC Embedded Controller "
				"USB Type-C Control");

	for (i = 0; i < num_ports; ++i) {
		rv = google_chromeec_get_pd_port_caps(i, &port_caps);
		if (rv)
			continue;

		con_name[3] = (char)i + '0';
		acpigen_write_device(con_name);
		acpigen_write_name_integer("_ADR", i);

		/* _DSD, Device-Specific Data */
		dsd = acpi_dp_new_table("_DSD");

		acpi_dp_add_integer(dsd, "port-number", i);
		add_port_caps(dsd, &port_caps);
		add_usb_port_references(dsd, i);

		acpi_dp_write(dsd);
		acpigen_pop_len(); /* Device CONx */
	}

	acpigen_pop_len(); /* Device GOOGLE_CHROMEEC_USBC_DEVICE_NAME */
}

void google_chromeec_fill_ssdt_generator(struct device *dev)
{
	int num_ports;
	if (google_chromeec_get_num_pd_ports(&num_ports))
		return;

	/* Reference the existing device's scope */
	acpigen_write_scope(acpi_device_path(dev));
	fill_ssdt_typec_device(num_ports);
	acpigen_pop_len(); /* Scope */
}
