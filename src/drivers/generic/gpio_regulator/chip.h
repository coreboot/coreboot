/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __DRIVERS_GENERIC_GPIO_REGULATOR_H__
#define __DRIVERS_GENERIC_GPIO_REGULATOR_H__

#include <arch/acpi_device.h>

struct drivers_generic_gpio_regulator_config {
	const char *name;
	struct acpi_gpio gpio;
	bool enabled_on_boot;
};

#endif /* __DRIVERS_GENERIC_GPIO_REGULATOR_H__ */
