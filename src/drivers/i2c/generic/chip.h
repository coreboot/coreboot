/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef __I2C_GENERIC_CHIP_H__
#define __I2C_GENERIC_CHIP_H__

#include <arch/acpi_device.h>
#include <device/i2c.h>

enum power_mgmt_type {
	POWER_RESOURCE = 1,
	GPIO_EXPORT = 2,
};

struct drivers_i2c_generic_config {
	const char *hid;	/* ACPI _HID (required) */
	const char *cid;	/* ACPI _CID */
	const char *name;	/* ACPI Device Name */
	const char *desc;	/* Device Description */
	unsigned uid;		/* ACPI _UID */
	enum i2c_speed speed;	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	unsigned wake;		/* Wake GPE */
	struct acpi_irq irq;	/* Interrupt */

	/*
	 * This flag will add a device propery which will indicate
	 * to the OS that it should probe this device before adding it.
	 *
	 * This can be used to declare a device that may not exist on
	 * the board, for example to support multiple trackpad vendors.
	 */
	int probed;

	/* GPIO used to indicate if this device is present */
	unsigned device_present_gpio;
	unsigned device_present_gpio_invert;

	/* Power management type. */
	enum power_mgmt_type pwr_mgmt_type;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned reset_delay_ms;
	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned enable_delay_ms;
};

/*
 * Fills in generic information about i2c device from device-tree
 * properties. Callback can be provided to fill in any
 * device-specific information in SSDT.
 *
 * Parameters:
 * dev: Device requesting i2c generic information to be filled
 * callback: Callback to fill in device-specific information
 * config: Pointer to drivers_i2c_generic_config structure
 */
void i2c_generic_fill_ssdt(struct device *dev,
			void (*callback)(struct device *dev),
			struct drivers_i2c_generic_config *config);

#endif /* __I2C_GENERIC_CHIP_H__ */
