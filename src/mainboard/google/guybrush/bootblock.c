/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio.h>

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_bootblock_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);

	if (CONFIG(GPIO_SIGN_OF_LIFE)) {
		for (int x = 0; x < 20; x++) {
			gpio_set(GPIO_31, 1);
			mdelay(10);
			gpio_set(GPIO_31, 0);
			mdelay(10);
		}
	}
}
