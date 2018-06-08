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

#include <intelblocks/sd.h>
#include "chip.h"

int sd_fill_soc_gpio_info(struct acpi_gpio* gpio, struct device *dev)
{
	config_t *config = dev->chip_info;

	if (!config->sdcard_cd_gpio)
		return -1;

	gpio->type = ACPI_GPIO_TYPE_INTERRUPT;
	gpio->pull = ACPI_GPIO_PULL_NONE;
	gpio->irq.mode = ACPI_IRQ_EDGE_TRIGGERED;
	gpio->irq.polarity = ACPI_IRQ_ACTIVE_BOTH;
	gpio->irq.shared = ACPI_IRQ_SHARED;
	gpio->irq.wake = ACPI_IRQ_WAKE;
	gpio->interrupt_debounce_timeout = 10000; /* 100ms */
	gpio->pin_count = 1;
	gpio->pins[0] = config->sdcard_cd_gpio;

	return 0;
}
