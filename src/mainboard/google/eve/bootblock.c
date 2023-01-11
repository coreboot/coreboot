/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>

#include "gpio.h"

static void early_config_gpio(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void bootblock_mainboard_early_init(void)
{
	early_config_gpio();
}
