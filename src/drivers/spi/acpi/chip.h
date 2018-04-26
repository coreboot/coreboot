/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#ifndef __SPI_ACPI_CHIP_H__
#define __SPI_ACPI_CHIP_H__

#include <arch/acpi_device.h>

struct drivers_spi_acpi_config {
	const char *hid;		/* ACPI _HID (required) */
	const char *cid;		/* ACPI _CID */
	const char *name;		/* ACPI Device Name */
	const char *desc;		/* Device Description */
	unsigned uid;			/* ACPI _UID */
	unsigned speed;			/* Bus speed in Hz (default 1MHz) */
	const char *compat_string;	/* Compatible string for _HID=PRP0001 */
	struct acpi_irq irq;		/* Interrupt */
	unsigned wake;			/* Wake GPE */

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;

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
};

#endif /* __SPI_ACPI_CHIP_H__ */
