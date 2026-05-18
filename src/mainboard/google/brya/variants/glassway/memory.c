/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

uint8_t mb_get_channel_disable_mask(void)
{
	/*
	 * GPP_E19 High -> One RAM Chip
	 * GPP_E19 Low  -> Two RAM Chip
	 */
	if (gpio_get(GPP_E19)) {
		/* Disable all other channels except first two on each controller */
		return (BIT(2) | BIT(3));
	}

	return 0;
}
