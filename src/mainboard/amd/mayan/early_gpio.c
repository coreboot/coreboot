/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include "gpio.h"

/* TODO: Update for mayan */

/* GPIO pins used by coreboot should be initialized in bootblock */

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
