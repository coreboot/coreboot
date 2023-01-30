/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

/* GPIO pins used by coreboot should be initialized in bootblock */

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
