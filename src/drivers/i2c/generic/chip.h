/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __I2C_GENERIC_CHIP_H__
#define __I2C_GENERIC_CHIP_H__

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>

#define MAX_GENERIC_PROPERTY_LIST 10

struct drivers_i2c_generic_config {
	const char *hid;	/* ACPI _HID (required) */
	const char *cid;	/* ACPI _CID */
	const char *name;	/* ACPI Device Name */
	const char *desc;	/* Device Description */
	unsigned int uid;		/* ACPI _UID */
	enum i2c_speed speed;	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	const char *compat_string;	/* Compatible string for _HID=PRP0001 */
	unsigned int wake;		/* Wake GPE */
	struct acpi_irq irq;	/* Interrupt */

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;

	/*
	 * This flag will add a device property which will indicate
	 * to the OS that it should probe this device before adding it.
	 *
	 * This can be used to declare a device that may not exist on
	 * the board, for example to support multiple trackpad vendors.
	 */
	int probed;

	/*
	 * This flag will add a device property which will indicate
	 * that coreboot should attempt to detect the device on the i2c
	 * bus before generating a device entry in the SSDT.
	 *
	 * This can be used to declare a device that may not exist on
	 * the board, for example to support multiple touchpads and/or
	 * touchscreens.
	 */
	int detect;

	/* GPIO used to indicate if this device is present */
	unsigned int device_present_gpio;
	unsigned int device_present_gpio_invert;

	/* Disable reset and enable GPIO export in _CRS */
	bool disable_gpio_export_in_crs;

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
	/* GPIO used to stop operation of device. */
	struct acpi_gpio stop_gpio;
	/* Delay to be inserted after disabling stop. */
	unsigned int stop_delay_ms;
	/* Delay to be inserted after enabling stop. */
	unsigned int stop_off_delay_ms;

	/* Generic properties for exporting device-specific data to the OS */
	struct acpi_dp property_list[MAX_GENERIC_PROPERTY_LIST];
	int property_count;
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
void i2c_generic_fill_ssdt(const struct device *dev,
			void (*callback)(const struct device *dev),
			struct drivers_i2c_generic_config *config);

#endif /* __I2C_GENERIC_CHIP_H__ */
