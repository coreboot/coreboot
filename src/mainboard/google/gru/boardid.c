/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
#include <stdlib.h>
#include <soc/saradc.h>

/*
 * ID info:
 *  ID : Volts : ADC value : Bucket
 *  ==   =====   =========   ======
 *  0  : 0.074V: 37.888    : 0 - <=82
 *  1  : 0.211V: 108.032   : 82- <=136
 *  2  : 0.319V: 163.328   : 136-<=191
 *  3  : 0.427V: 218.624   : 191-<=248
 *  4  : 0.542V: 277.504   : 248-<=309
 *  5  : 0.666V: 340.992   : 309-<=370
 *  6  : 0.781V: 399.872   : 370-  512
 */
static const int id_readings[] = { 82, 136, 191, 248, 309, 370, 512 };
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

	printk(BIOS_DEBUG, "ERROR: Unmatched ADC reading of %d\n", adc_reading);
	return 0;
}

uint8_t board_id(void)
{
	return get_index(1, &cached_board_id);
}

uint32_t ram_code(void)
{
	return get_index(0, &cached_ram_id);
}
