/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
#include <gpio.h>

/* Gandof board memory configuration GPIOs */
#define SPD_GPIO_BIT0		13
#define SPD_GPIO_BIT1		9
#define SPD_GPIO_BIT2		47

unsigned int variant_get_spd_index(void)
{
	const gpio_t gpio_vector[] = {
		SPD_GPIO_BIT0,
		SPD_GPIO_BIT1,
		SPD_GPIO_BIT2
	};
	return gpio_base2_value(gpio_vector, ARRAY_SIZE(gpio_vector));
}

bool variant_is_dual_channel(const unsigned int spd_index)
{
	/* Index 0-2 are 4GB config with both CH0 and CH1.
	 * Index 4-6 are 2GB config with CH0 only. */
	return !(spd_index > 3);
}
