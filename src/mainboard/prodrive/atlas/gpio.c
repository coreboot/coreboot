/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#include "gpio.h"

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* Todo: gpio config */
};

void configure_gpio_pads(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
