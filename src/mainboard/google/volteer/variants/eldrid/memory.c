/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

/*This mb_ddr4_cfg structure is intentionally left empty so that fields are left nil. */
static const struct mb_ddr4_cfg eldrid_memcfg = {
};

static const struct ddr_memory_cfg baseboard_memcfg = {
	.mem_type = MEMTYPE_DDR4,
	.ddr4_cfg = &eldrid_memcfg
};

const struct ddr_memory_cfg *variant_memory_params(void)
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
