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

#ifndef __DRIVERS_GENERIC_GPIO_REGULATOR_H__
#define __DRIVERS_GENERIC_GPIO_REGULATOR_H__

#include <arch/acpi_device.h>

struct drivers_generic_gpio_regulator_config {
	const char *name;
	struct acpi_gpio gpio;
	bool enabled_on_boot;
};

#endif /* __DRIVERS_GENERIC_GPIO_REGULATOR_H__ */
