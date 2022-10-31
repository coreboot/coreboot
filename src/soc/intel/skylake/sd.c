/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <device/device.h>
#include <intelblocks/sd.h>
#include <stdint.h>

#include "chip.h"

int sd_fill_soc_gpio_info(struct acpi_gpio *gpio, const struct device *dev)
{
	config_t *config = config_of(dev);

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
