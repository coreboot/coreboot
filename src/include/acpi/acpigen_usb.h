/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef ACPI_ACPIGEN_USB_H
#define ACPI_ACPIGEN_USB_H

#include <acpi/acpi_pld.h>

enum usb_typec_power_role {
	TYPEC_POWER_ROLE_SOURCE,
	TYPEC_POWER_ROLE_SINK,
	TYPEC_POWER_ROLE_DUAL,
};

enum usb_typec_try_power_role {
	TYPEC_TRY_POWER_ROLE_NONE,
	TYPEC_TRY_POWER_ROLE_SINK,
	TYPEC_TRY_POWER_ROLE_SOURCE,
};

enum usb_typec_data_role {
	TYPEC_DATA_ROLE_DFP,
	TYPEC_DATA_ROLE_UFP,
	TYPEC_DATA_ROLE_DUAL,
};

/**
 * Configuration required to write out a Type-C Connector ACPI object.
 *
 * @power_role: DUAL if device supports being both a source and a sink, otherwise choose
 *	the device's default power role
 * @try_power_role: SINK if device supports Try.SNK, SOURCE if device supports Try.SRC,
 *	otherwise choose NONE
 * @data_role: Choose DUAL if device can alternate between UFP (host) & DFP (device),
 *	otherwise specify UFP or DFP.
 * @usb2_port: Reference to the ACPI device that represents the USB2 signals
 * @usb3_port: Reference to the ACPI device that represents the USB3 signals
 * @usb4_port: Reference to the ACPI device that represents the USB4 signals
 * @orientation_switch: Reference to the ACPI device that controls the switching of
 *	the orientation/polarity for Data and SBU lines.
 * @usb_role_switch: Reference to the ACPI device that can select the USB role,
 *	host or device, for the USB port
 * @mode_switch: Reference to the ACPI device that controls routing of data lines to
 *	various endpoints (xHCI, DP, etc.) on the SoC.
 * @retimer_switch: Reference to the ACPI device that controls the configuration
 * of the retimer in the Type C signal chain.
 * @pld: Reference to PLD information.
 */
struct typec_connector_class_config {
	enum usb_typec_power_role power_role;
	enum usb_typec_try_power_role try_power_role;
	enum usb_typec_data_role data_role;
	const struct device *usb2_port;
	const struct device *usb3_port;
	const struct device *usb4_port;
	const struct device *orientation_switch;
	const struct device *usb_role_switch;
	const struct device *mode_switch;
	const struct device *retimer_switch;
	const struct acpi_pld *pld;
};

typedef void (*add_custom_dsd_property_cb)(struct acpi_dp *dsd, int port_number);

void acpigen_write_typec_connector(const struct typec_connector_class_config *config,
				   int port_number,
				   add_custom_dsd_property_cb add_custom_dsd_property);

#endif /* ACPI_ACPIGEN_USB_H */
