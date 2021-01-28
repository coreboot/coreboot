/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <endian.h>
#include <string.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <soc/pei_data.h>
#include <soc/romstage.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/google/auron/ec.h>
#include <mainboard/google/auron/variant.h>

/* Lulu board memory configuration GPIOs */
#define SPD_GPIO_BIT0		13
#define SPD_GPIO_BIT1		9
#define SPD_GPIO_BIT2		47
#define SPD_GPIO_BIT3		8

/* Copy SPD data for on-board memory */
void mainboard_fill_spd_data(struct pei_data *pei_data)
{
	const int gpio_vector[] = {
		SPD_GPIO_BIT0,
		SPD_GPIO_BIT1,
		SPD_GPIO_BIT2,
		SPD_GPIO_BIT3,
		-1,
	};

	const unsigned int spd_index = get_gpios(gpio_vector);

	/* CH0 */
	fill_spd_for_index(pei_data->spd_data[0][0], spd_index);

	/* CH1 not used in 2GB configurations */
	if (!((spd_index == 0b0000) || (spd_index == 0b0011) ||
	    (spd_index == 0b1010))) {
		memcpy(pei_data->spd_data[1][0], pei_data->spd_data[0][0], SPD_LEN);
	}
}
