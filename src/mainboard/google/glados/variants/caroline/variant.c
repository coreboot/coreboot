/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2016 Intel Corporation
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

#include <baseboard/variant.h>
#include <gpio.h>
#include <stdint.h>
#include <string.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <variant/gpio.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		    0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 4, 5, 6, 7 },
		{ 1, 0, 4, 5, 2, 3, 6, 7 } };

	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };

	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 40, 23, 40 };

	memcpy(pei_data->dq_map, dq_map, sizeof(dq_map));
	memcpy(pei_data->dqs_map, dqs_map, sizeof(dqs_map));
	memcpy(pei_data->RcompResistor, RcompResistor,
		 sizeof(RcompResistor));
	memcpy(pei_data->RcompTarget, RcompTarget,
		 sizeof(RcompTarget));
}

void mainboard_gpio_smi_sleep(void)
{
	int i;

	/* Power down the rails on any sleep type. */
	gpio_t active_high_signals[] = {
		EN_PP3300_DX_DIG,
		EN_PP3300_DX_TOUCH,
		EN_PP3300_DX_EMMC,
		EN_PP1800_DX_EMMC,
		EN_PP3300_DX_CAM,
	};

	for (i = 0; i < ARRAY_SIZE(active_high_signals); i++)
		gpio_set(active_high_signals[i], 0);
}
