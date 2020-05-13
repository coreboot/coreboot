/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg baseboard_memcfg_cfg = {

	.dq_map[DDR_CH0] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x0, 0x0},
		{0x0, 0x0}
	},
	.dq_map[DDR_CH1] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x00, 0x0},
		{0x00, 0x0}
	},

	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on SoC
	 * the value = pin number on LPDDR4 part
	 */

	.dqs_map[DDR_CH0] = {1, 3, 0, 2, 7, 5, 4, 6},
	.dqs_map[DDR_CH1] = {3, 1, 2, 0, 4, 5, 7, 6},

	/* WaddleDoo uses 100, 100 and 100 rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/* WaddleDoo Rcomp target values */
	.rcomp_targets = {80, 40, 40, 40, 30},

	/* Disable Early Command Training */
	.ect = 1,

	/* User Board Type */
	.UserBd = BOARD_TYPE_MOBILE,
};

const struct mb_cfg *__weak variant_memcfg_config(void)
{
	return &baseboard_memcfg_cfg;
}

int __weak variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool __weak variant_mem_is_half_populated(void)
{
	return !gpio_get(GPIO_MEM_CH_SEL);
}
