/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <assert.h>
#include <boardid.h>
#include <soc/auxadc.h>
#include <stddef.h>

static uint32_t get_index(unsigned int channel, uint32_t *cached_id)
{
	static const int voltages[] = {
	/* ID : Voltage (unit: uV) */
	/*  0 : */   74000,
	/*  1 : */  212000,
	/*  2 : */  319000,
	/*  3 : */  429000,
	/*  4 : */  542000,
	/*  5 : */  666000,
	/*  6 : */  781000,
	/*  7 : */  900000,
	/*  8 : */ 1023000,
	/*  9 : */ 1137000,
	/* 10 : */ 1240000,
	/* 11 : */ 1343000,
	/* 12 : */ 1457000,
	/* 13 : */ 1576000,
	/* 14 : */ 1684000,
	/* 15 : */ 1800000,
	};

	uint32_t id;

	if (*cached_id != BOARD_ID_INIT)
		return *cached_id;

	int value = auxadc_get_voltage(channel);
	/* Find the closest voltage */
	for (id = 0; id < ARRAY_SIZE(voltages) - 1; id++)
		if (value < (voltages[id] + voltages[id + 1]) / 2)
			break;

	const int tolerance = 10000; /* 10,000 uV */
	assert(ABS(value - voltages[id]) < tolerance);

	*cached_id = id;
	return id;
}

/* board_id is provided by ec/google/chromeec/ec_boardid.c */

uint32_t sku_id(void)
{
	static uint32_t cached_sku_id = BOARD_ID_INIT;

	/* Quirk for KUKUI: All P1/SKU0 had incorrectly set SKU=1. */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KUKUI)) {
		if (cached_sku_id == BOARD_ID_INIT && board_id() == 1) {
			cached_sku_id = 0;
			return cached_sku_id;
		}
	}
	return get_index(4, &cached_sku_id);
}

uint32_t ram_code(void)
{
	static uint32_t cached_ram_code = BOARD_ID_INIT;
	return get_index(3, &cached_ram_code);
}
