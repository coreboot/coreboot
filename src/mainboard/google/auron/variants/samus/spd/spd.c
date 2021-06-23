/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
#include <soc/pei_wrapper.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>

/* Samus board memory configuration GPIOs */
#define SPD_GPIO_BIT0		69
#define SPD_GPIO_BIT1		68
#define SPD_GPIO_BIT2		67
#define SPD_GPIO_BIT3		65

unsigned int variant_get_spd_index(void)
{
	const int gpio_vector[] = {
		SPD_GPIO_BIT0,
		SPD_GPIO_BIT1,
		SPD_GPIO_BIT2,
		SPD_GPIO_BIT3,
		-1,
	};
	return get_gpios(gpio_vector);
}

bool variant_is_dual_channel(const unsigned int spd_index)
{
	/* Assume same memory in both channels */
	return true;
}

const struct lpddr3_dq_dqs_map *mb_get_lpddr3_dq_dqs_map(void)
{
	static const struct lpddr3_dq_dqs_map lpddr3_map = {
		.dq = {
			{ { 0x0F, 0xF0 }, { 0x00, 0xF0 }, { 0x0F, 0xF0 },
			  { 0x0F, 0x00 }, { 0xFF, 0x00 }, { 0xFF, 0x00 } },
			{ { 0x0F, 0xF0 }, { 0x00, 0xF0 }, { 0x0F, 0xF0 },
			  { 0x0F, 0x00 }, { 0xFF, 0x00 }, { 0xFF, 0x00 } },
		},
		.dqs = {
			{ 2, 0, 1, 3, 6, 4, 7, 5 },
			{ 2, 1, 0, 3, 6, 5, 4, 7 },
		},
	};
	return &lpddr3_map;
}
