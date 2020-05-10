/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/sd.h>
#include "chip.h"

int sd_fill_soc_gpio_info(struct acpi_gpio* gpio, const struct device *dev)
{
	config_t *config = config_of(dev);

	/* Nothing to write if GPIO is not set in devicetree */
	if(!config->sdcard_cd_gpio_default && !config->sdcard_cd_gpio.pins[0])
		return -1;

	if (config->sdcard_cd_gpio_default) {
		gpio->type = ACPI_GPIO_TYPE_INTERRUPT;
		gpio->pull = ACPI_GPIO_PULL_NONE;
		gpio->irq.mode = ACPI_IRQ_EDGE_TRIGGERED;
		gpio->irq.polarity = ACPI_IRQ_ACTIVE_BOTH;
		gpio->irq.shared = ACPI_IRQ_SHARED;
		gpio->irq.wake = ACPI_IRQ_WAKE;
		gpio->interrupt_debounce_timeout = 10000; /* 100ms */
		gpio->pin_count = 1;
		gpio->pins[0] = config->sdcard_cd_gpio_default;
	} else
		*gpio = config->sdcard_cd_gpio;

	return 0;
}
