/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INTEL_MIPI_CAMERA_PMIC_CHIP_H__
#define __INTEL_MIPI_CAMERA_PMIC_CHIP_H__

#include <stdint.h>
#include <acpi/acpi_device.h>
#include "cldb.h"

#define MAX_MIPICAM_GPIO_CONFIGS	6

enum mipicam_gpio_type {
	MIPICAM_GPIO_TYPE_RESET = 0x00,
	MIPICAM_GPIO_TYPE_POWER = 0x0B,
	MIPICAM_GPIO_TYPE_CLOCK = 0x0C,
	MIPICAM_GPIO_TYPE_PLED = 0x0D,
	MIPICAM_GPIO_TYPE_STROBE = 0x02,
	MIPICAM_GPIO_TYPE_HANDSHAKE = 0x12,
	MIPICAM_GPIO_TYPE_HOTPLUG_DETECT = 0x13
};

struct mipicam_gpio_ctrl_panel {
	struct acpi_gpio gpio;
	uint8_t gpio_function;
	uint8_t gpio_active_value;
	uint8_t gpio_initial_value;
};

struct drivers_intel_mipi_camera_pmic_config {
	struct intel_cldb cldb;
	const char *acpi_hid;
	const char *acpi_name;
	unsigned int acpi_uid;

	/* If set, _STA returns this object (e.g. "\\_SB.PCI0.CSTA") */
	const char *acpi_sta;

	/* GPIO used for camera in discrete mode */
	struct mipicam_gpio_ctrl_panel gpio_panel[MAX_MIPICAM_GPIO_CONFIGS];

	/* Device has a power resource */
	bool has_power_resource;

	/* Pulse reset_gpio only on the first _ON after boot */
	bool reset_once;

	/* GPIO used to take device out of reset or put it into reset */
	struct acpi_gpio reset_gpio;
	/* Delay after device is taken out of reset */
	unsigned int reset_delay_ms;
	/* Delay after device is put into reset */
	unsigned int reset_off_delay_ms;
	/* GPIO used to enable device */
	struct acpi_gpio enable_gpio;
	/* Delay after device is enabled */
	unsigned int enable_delay_ms;
	/* Delay after device is disabled */
	unsigned int enable_off_delay_ms;
	/* GPIO used to stop operation of device */
	struct acpi_gpio stop_gpio;
	/* Delay after disabling stop */
	unsigned int stop_delay_ms;
	/* Delay after enabling stop */
	unsigned int stop_off_delay_ms;

	/* If true, _STA method will use the state of the GPIOs to determine if the
	 * PowerResource is ON or OFF.
	 * If false, the _STA method will always return ON.
	 */
	bool use_gpio_for_status;
};

#endif
