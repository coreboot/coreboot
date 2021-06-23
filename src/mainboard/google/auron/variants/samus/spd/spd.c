/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
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
