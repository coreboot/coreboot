/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_DDR4,
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_3,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_0,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
