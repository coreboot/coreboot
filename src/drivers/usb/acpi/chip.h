/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __USB_ACPI_CHIP_H__
#define __USB_ACPI_CHIP_H__

#include <acpi/acpi_device.h>
#include <acpi/acpi.h>
#include <acpi/acpi_pld.h>

struct drivers_usb_acpi_config {
	const char *desc;

	/*
	 * Physical ports that are user visible
	 *
	 * UPC_TYPE_A
	 * UPC_TYPE_MINI_AB
	 * UPC_TYPE_EXPRESSCARD
	 * UPC_TYPE_USB3_A
	 * UPC_TYPE_USB3_B
	 * UPC_TYPE_USB3_MICRO_B
	 * UPC_TYPE_USB3_MICRO_AB
	 * UPC_TYPE_USB3_POWER_B
	 * UPC_TYPE_C_USB2_ONLY
	 * UPC_TYPE_C_USB2_SS_SWITCH
	 * UPC_TYPE_C_USB2_SS
	 *
	 * Non-visible ports or special devices
	 *
	 * UPC_TYPE_PROPRIETARY
	 * UPC_TYPE_INTERNAL
	 * UPC_TYPE_UNUSED
	 * UPC_TYPE_HUB
	 */
	enum acpi_upc_type type;

	/* Group peer ports */
	struct acpi_pld_group group;

	/*
	 * Define a custom physical location for the port.
	 * If enabled, this takes precedence over the 'group' field.
	 */
	bool use_custom_pld;
	struct acpi_pld custom_pld;

	/* Does the device have a power resource? */
	bool has_power_resource;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;
	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;

	/*
	 * Define a GPIO that shows the privacy status of the USB device.
	 * E.g. On a camera: if it is one, it is recording black frames.
	 * E.g. On a mic: if it is one, it is recording white-noise.
	 */
	struct acpi_gpio privacy_gpio;

	/* Write a _STA method that uses the state of the GPIOs to determine if
	 * the PowerResource is ON or OFF. If this is false, the _STA method
	 * will always return ON.
	 */
	bool use_gpio_for_status;
};

/* Method to get PLD structure from USB device */
bool usb_acpi_get_pld(const struct device *usb_device, struct acpi_pld *pld);

#endif /* __USB_ACPI_CHIP_H__ */
