/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_R8168_CHIP_H__
#define __DRIVERS_R8168_CHIP_H__

#include <stdint.h>
#include <acpi/acpi_device.h>

struct drivers_net_config {
	uint16_t customized_leds;
	/* RTL8125 LED settings */
	uint8_t led_feature;
	uint16_t customized_led0;
	uint16_t customized_led2;

	unsigned int wake;	/* Wake pin for ACPI _PRW */

	/* Does the device have a power resource? */
	bool has_power_resource;

	/* GPIO used to stop operation of device. */
	struct acpi_gpio stop_gpio;
	/* Delay to be inserted after disabling stop. */
	unsigned int stop_delay_ms;
	/* Delay to be inserted after enabling stop. */
	unsigned int stop_off_delay_ms;

	/*
	 * There maybe many NIC cards in a system.
	 * This parameter is for driver to identify what
	 * the device number is and the valid range is [1-10].
	 */
	uint8_t device_index;
};

#endif /* __DRIVERS_R8168_CHIP_H__ */
