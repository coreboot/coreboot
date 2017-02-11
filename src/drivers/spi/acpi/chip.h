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
	unsigned speed;		/* Bus speed in Hz (default 1MHz) */
	const char *compat_string;	/* Compatible string for _HID=PRP0001 */
	struct acpi_irq irq;		/* Interrupt */
};

#endif /* __SPI_ACPI_CHIP_H__ */
