/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

uint8_t mb_get_channel_disable_mask(void)
{
	/*
	 * GPP_E9 High -> Single RAM Chip
	 * GPP_E9 Low  -> Dual RAM Chip
	 */
	if (gpio_get(GPP_E9)) {
		/* Disable all other channels except first two on each controller */
		printk(BIOS_INFO, "Device only supports one DIMM. Disable all other memory"
		"channels except first two on each memory controller.\n");
		return (BIT(2) | BIT(3));
	}
	return 0;

}
