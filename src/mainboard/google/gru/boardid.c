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
 * This matches Kevin/Gru ADC value for board id.
 */
static const int board_id_readings[] = { 42, 120, 181, 242, 307, 378, 444,
                                         511, 581, 646, 704, 763, 828,
                                         895, 956, 1023 };

/*
 * The ADC produces a 10 bit value, the resistor accuracy is 1%, let's leave
 * 2% room for error on both sides, total variation would be 4%, which is
 * approximately 40 points with a 10 bit ADC. The hardware specification
 * guarantees valid readings to be at least 64 bits (2^6) apart.
 */
#define ACCEPTABLE_DELTA  (int)(1024 * .02)

uint8_t board_id(void)
{
	static int cached_board_id = -1;
	int i;
	int adc_reading;

	if (cached_board_id != -1)
		return cached_board_id;

	adc_reading = get_saradc_value(1);
	for (i = 0; i < ARRAY_SIZE(board_id_readings); i++) {
		int delta = board_id_readings[i] - adc_reading;

		if ((delta < ACCEPTABLE_DELTA) && (delta > -ACCEPTABLE_DELTA)) {
			printk(BIOS_DEBUG, "ADC reading %d, "
			       "expected value %d board ID %d\n",
			       adc_reading, delta + adc_reading, i);
			cached_board_id = i;
			return i;
		}
	}

	printk(BIOS_ERR, "Unmatched ADC reading of %d, using Board ID of 0\n",
	       adc_reading);
	return 0;
}

uint32_t ram_code(void)
{
	return 0;
}
