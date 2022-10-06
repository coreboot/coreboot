/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include "gpio.h"

/* TODO: Update for birman */

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_ram[] = {

};

void mainboard_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_ram, ARRAY_SIZE(gpio_set_stage_ram));
}
