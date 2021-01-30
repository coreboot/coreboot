/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <endian.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/romstage.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/google/auron/ec.h>
#include <mainboard/google/auron/variant.h>

/* Samus board memory configuration GPIOs */
#define SPD_GPIO_BIT0		69
#define SPD_GPIO_BIT1		68
#define SPD_GPIO_BIT2		67
#define SPD_GPIO_BIT3		65

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

	fill_spd_for_index(pei_data->spd_data[0][0], spd_index);

	/* Assume same memory in both channels */
	memcpy(pei_data->spd_data[1][0], pei_data->spd_data[0][0], SPD_LEN);
}
