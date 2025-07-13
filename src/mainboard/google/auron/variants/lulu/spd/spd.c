/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
#include <gpio.h>

/* Lulu board memory configuration GPIOs */
#define SPD_GPIO_BIT0		13
#define SPD_GPIO_BIT1		9
#define SPD_GPIO_BIT2		47
#define SPD_GPIO_BIT3		8

unsigned int variant_get_spd_index(void)
{
	const gpio_t gpio_vector[] = {
		SPD_GPIO_BIT0,
		SPD_GPIO_BIT1,
		SPD_GPIO_BIT2,
		SPD_GPIO_BIT3
	};
	return gpio_base2_value(gpio_vector, ARRAY_SIZE(gpio_vector));
}

bool variant_is_dual_channel(const unsigned int spd_index)
{
	/* CH1 not used in 2GB configurations */
	return !((spd_index == 0b0000) || (spd_index == 0b0011) || (spd_index == 0b1010));
}
