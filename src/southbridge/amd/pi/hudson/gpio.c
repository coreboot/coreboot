/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include "gpio.h"

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);

	return !!(reg & GPIO_PIN_STS);
}
