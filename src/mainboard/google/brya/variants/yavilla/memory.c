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
		printk(BIOS_INFO, "Disable all other memory channels"
		"except first two on each memory controller.\n");
		return (BIT(2) | BIT(3));
	}

	return 0;
}

int variant_memory_sku(void)
{
	/*
	* Memory configuration board straps
	* GPIO_MEM_CONFIG_0    GPP_E1
	* GPIO_MEM_CONFIG_1    GPP_E2
	* GPIO_MEM_CONFIG_2    GPP_E3
	* GPIO_MEM_CONFIG_3    GPP_F7
	*/
	gpio_t spd_gpios[] = {
		GPP_E1,
		GPP_E2,
		GPP_E3,
		GPP_F7,
        };

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
