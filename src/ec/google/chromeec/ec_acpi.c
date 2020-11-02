/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_ps2_keybd.h>
#include <acpi/acpigen_usb.h>
#include <console/console.h>
#include <drivers/usb/acpi/chip.h>
#include <ec/google/common/dptf.h>

#include "chip.h"
#include "ec.h"
#include "ec_commands.h"

#define GOOGLE_CHROMEEC_USBC_DEVICE_HID		"GOOG0014"
#define GOOGLE_CHROMEEC_USBC_DEVICE_NAME	"USBC"

const char *google_chromeec_acpi_name(const struct device *dev)
{
	/*
	 * Chrome EC device (CREC - GOOG0004) is really a child of EC device (EC - PNP0C09) in
	 * ACPI tables. However, in coreboot device tree, there is no separate chip/device for
	 * EC0. Thus, Chrome EC device needs to return "EC0.CREC" as the ACPI name so that the
	 * callers can get the correct acpi device path/scope for this device.
	 *
	 * If we ever enable a separate driver for generating AML for EC0 device, then this
	 * function needs to be updated to return "CREC".
	 */
	return "EC0.CREC";
}

/*
 * Helper for fill_ssdt_generator. This adds references to the USB
 * port objects so that the consumer of this information can know
 * whether the port supports USB2 and/or USB3.
 */
static void get_usb_port_references(int port_number, struct device **usb2_port,
				    struct device **usb3_port, struct device **usb4_port)
{
	struct drivers_usb_acpi_config *config;
	struct device *port = NULL;

	/* Search through the devicetree for matching USB Type-C ports */
	while ((port = dev_find_path(port, DEVICE_PATH_USB)) != NULL) {
		if (!port->enabled || port->path.type != DEVICE_PATH_USB)
			continue;

		config = port->chip_info;

		/* Look at only USB Type-C ports */
		if ((config->type != UPC_TYPE_C_USB2_ONLY) &&
		    (config->type != UPC_TYPE_C_USB2_SS_SWITCH) &&
		    (config->type != UPC_TYPE_C_USB2_SS))
			continue;

		/*
		 * Check for a matching port number (the 'token' field in 'group').  Note that
		 * 'port_number' is 0-based, whereas the 'token' field is 1-based.
		 */
		if (config->group.token != (port_number + 1))
			continue;

		switch (port->path.usb.port_type) {
		case 2:
			*usb2_port = port;
			break;
		case 3:
			*usb3_port = port;
			break;
		case 4:
			*usb4_port = port;
			break;
		default:
			break;
		}
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

static struct usb_pd_port_caps port_caps;
static void add_port_location(struct acpi_dp *dsd, int port_number)
{
	acpi_dp_add_string(dsd, "port-location", port_location_to_str(port_caps.port_location));
}

static void fill_ssdt_typec_device(const struct device *dev)
{
	struct ec_google_chromeec_config *config = dev->chip_info;
	int rv;
	int i;
	unsigned int num_ports;
	struct device *usb2_port;
	struct device *usb3_port;
	struct device *usb4_port;

	if (google_chromeec_get_num_pd_ports(&num_ports))
		return;

	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_device(GOOGLE_CHROMEEC_USBC_DEVICE_NAME);
	acpigen_write_name_string("_HID", GOOGLE_CHROMEEC_USBC_DEVICE_HID);
	acpigen_write_name_string("_DDN", "ChromeOS EC Embedded Controller "
				"USB Type-C Control");

	for (i = 0; i < num_ports; ++i) {
		rv = google_chromeec_get_pd_port_caps(i, &port_caps);
		if (rv)
			continue;

		usb2_port = NULL;
		usb3_port = NULL;
		usb4_port = NULL;
		get_usb_port_references(i, &usb2_port, &usb3_port, &usb4_port);

		struct typec_connector_class_config typec_config = {
			.power_role = port_caps.power_role_cap,
			.try_power_role = port_caps.try_power_role_cap,
			.data_role = port_caps.data_role_cap,
			.usb2_port = usb2_port,
			.usb3_port = usb3_port,
			.usb4_port = usb4_port,
			.orientation_switch = config->mux_conn[i],
			.usb_role_switch = config->mux_conn[i],
			.mode_switch = config->mux_conn[i],
		};

		acpigen_write_typec_connector(&typec_config, i, add_port_location);
	}

	acpigen_pop_len(); /* Device GOOGLE_CHROMEEC_USBC_DEVICE_NAME */
	acpigen_pop_len(); /* Scope */
}

static const enum ps2_action_key ps2_enum_val[] = {
	[TK_ABSENT] = PS2_KEY_ABSENT,
	[TK_BACK] = PS2_KEY_BACK,
	[TK_FORWARD] = PS2_KEY_FORWARD,
	[TK_REFRESH] = PS2_KEY_REFRESH,
	[TK_FULLSCREEN] = PS2_KEY_FULLSCREEN,
	[TK_OVERVIEW] = PS2_KEY_OVERVIEW,
	[TK_BRIGHTNESS_DOWN] = PS2_KEY_BRIGHTNESS_DOWN,
	[TK_BRIGHTNESS_UP] = PS2_KEY_BRIGHTNESS_UP,
	[TK_VOL_MUTE] = PS2_KEY_VOL_MUTE,
	[TK_VOL_DOWN] = PS2_KEY_VOL_DOWN,
	[TK_VOL_UP] = PS2_KEY_VOL_UP,
	[TK_SNAPSHOT] = PS2_KEY_SNAPSHOT,
	[TK_PRIVACY_SCRN_TOGGLE] = PS2_KEY_PRIVACY_SCRN_TOGGLE,
	[TK_KBD_BKLIGHT_DOWN] = PS2_KEY_KBD_BKLIGHT_DOWN,
	[TK_KBD_BKLIGHT_UP] = PS2_KEY_KBD_BKLIGHT_UP,
	[TK_PLAY_PAUSE] = PS2_KEY_PLAY_PAUSE,
	[TK_NEXT_TRACK] = PS2_KEY_NEXT_TRACK,
	[TK_PREV_TRACK] = PS2_KEY_PREV_TRACK,
};

static void fill_ssdt_ps2_keyboard(const struct device *dev)
{
	uint8_t i;
	struct ec_response_keybd_config keybd = {};
	enum ps2_action_key ps2_action_keys[MAX_TOP_ROW_KEYS] = {};

	if (google_chromeec_get_keybd_config(&keybd) ||
	    !keybd.num_top_row_keys ||
	    keybd.num_top_row_keys > MAX_TOP_ROW_KEYS) {
		printk(BIOS_ERR, "PS2K: Bad resp from EC. Vivaldi disabled!\n");
		return;
	}

	/* Convert enum action_key values to enum ps2_action_key values */
	for (i = 0; i < keybd.num_top_row_keys; i++)
		ps2_action_keys[i] = ps2_enum_val[keybd.action_keys[i]];

	acpigen_ps2_keyboard_dsd("_SB.PCI0.PS2K", keybd.num_top_row_keys,
				 ps2_action_keys,
				 !!(keybd.capabilities & KEYBD_CAP_FUNCTION_KEYS),
				 !!(keybd.capabilities & KEYBD_CAP_NUMERIC_KEYPAD),
				 !!(keybd.capabilities & KEYBD_CAP_SCRNLOCK_KEY));
}

static const char *ec_acpi_name(const struct device *dev)
{
	return "EC0";
}

static struct device_operations ec_ops = {
	.acpi_name	= ec_acpi_name,
};

void google_chromeec_fill_ssdt_generator(const struct device *dev)
{
	struct device_path path;
	struct device *ec;

	/* Set up a minimal EC0 device to pass to the DPTF helpers */
	path.type = DEVICE_PATH_GENERIC;
	path.generic.id = 0;
	ec = alloc_find_dev(dev->bus, &path);
	ec->ops = &ec_ops;

	if (CONFIG(DRIVERS_INTEL_DPTF))
		ec_fill_dptf_helpers(ec);

	fill_ssdt_typec_device(dev);
	fill_ssdt_ps2_keyboard(dev);
}
