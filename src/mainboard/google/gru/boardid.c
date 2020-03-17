/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/saradc.h>

static const int id_readings[] = {
/*	ID : Volts : ADC value :    Bucket	*/
/*	==   =====   =========    ==========	*/
#if CONFIG(BOARD_GOOGLE_KEVIN)
/*	 0 : 0.109V:        62 :    0 -   91	*/	91,
#else
/*	 0 : 0.074V:        42 :    0 -   81	*/	81,
#endif
/*	 1 : 0.211V:       120 :   82 -  150	*/	150,
/*	 2 : 0.319V:       181 :  151 -  211	*/	211,
/*	 3 : 0.427V:       242 :  212 -  274	*/	274,
/*	 4 : 0.542V:       307 :  275 -  342	*/	342,
/*	 5 : 0.666V:       378 :  343 -  411	*/	411,
/*	 6 : 0.781V:       444 :  412 -  477	*/	477,
/*	 7 : 0.900V:       511 :  478 -  545	*/	545,
/*	 8 : 1.023V:       581 :  546 -  613	*/	613,
/*	 9 : 1.137V:       646 :  614 -  675	*/	675,
/*	10 : 1.240V:       704 :  676 -  733	*/	733,
/*	11 : 1.343V:       763 :  734 -  795	*/	795,
/*	12 : 1.457V:       828 :  796 -  861	*/	861,
/*	13 : 1.576V:       895 :  862 -  925	*/	925,
/*	14 : 1.684V:       956 :  926 -  989	*/	989,
/*	15 : 1.800V:      1023 :  990 - 1023	*/	1023
};
_Static_assert(ARRAY_SIZE(id_readings) == 16, "Yo' messed up da table, bruh!");
static int cached_board_id = -1;
static int cached_ram_id = -1;

static uint32_t get_index(uint32_t channel, int *cached_id)
{
	int i;
	int adc_reading;

	if (*cached_id != -1)
		return *cached_id;

	adc_reading = get_saradc_value(channel);
	for (i = 0; i < ARRAY_SIZE(id_readings); i++) {
		if (adc_reading <= id_readings[i]) {
			printk(BIOS_DEBUG, "ADC reading %d, ID %d\n",
			       adc_reading, i);
			*cached_id = i;
			return i;
		}
	}

	die("Read impossible value (> 1023) from 10-bit ADC!");
}

uint32_t board_id(void)
{
	return get_index(1, &cached_board_id);
}

uint32_t ram_code(void)
{
	return get_index(0, &cached_ram_id);
}

uint32_t sku_id(void)
{
	if (!CONFIG(GRU_BASEBOARD_SCARLET))
		return UNDEFINED_STRAPPING_ID;

	static uint32_t sku_id = UNDEFINED_STRAPPING_ID;
	if (sku_id != UNDEFINED_STRAPPING_ID)
		return sku_id;

	gpio_t pins[3] = {[2] = GPIO(3, D, 6), [1] = GPIO(3, D, 5),
			  [0] = GPIO(3, D, 4)}; /* GPIO3_D4 is LSB */

	sku_id = gpio_pullup_base2_value(pins, ARRAY_SIZE(pins));
	return sku_id;
}
