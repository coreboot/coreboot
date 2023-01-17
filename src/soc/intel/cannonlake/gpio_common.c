/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <soc/soc_chip.h>

/*
 * Routine to perform below operations:
 * 1. SoC routine to fill GPIO PM mask and value for GPIO_MISCCFG register
 * 2. Program GPIO PM configuration based on PM mask and value
 */
void soc_gpio_pm_configuration(void)
{
	uint8_t value[TOTAL_GPIO_COMM];
	const config_t *config = config_of_soc();

	if (config->gpio_override_pm)
		memcpy(value, config->gpio_pm, sizeof(value));
	else
		memset(value, MISCCFG_GPIO_PM_CONFIG_BITS, sizeof(value));

	gpio_pm_configure(value, TOTAL_GPIO_COMM);
}
