/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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
#include <fsp/soc_binding.h>
#include <gpio.h>
#include <stdint.h>
#include <string.h>
#include <variant/gpio.h>

void variant_memory_init_params(
		MEMORY_INIT_UPD *const memory_params, const int spd_index)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		    0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 3, 1, 2, 4, 5, 6, 7 },
		{ 1, 0, 4, 5, 2, 3, 6, 7 } };

	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };

	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 40, 23, 40 };

	memcpy(memory_params->DqByteMapCh0, dq_map,
			sizeof(memory_params->DqByteMapCh0) * 2);
	memcpy(memory_params->DqsMapCpu2DramCh0, dqs_map,
			sizeof(memory_params->DqsMapCpu2DramCh0) * 2);
	memcpy(memory_params->RcompResistor, RcompResistor,
			sizeof(memory_params->RcompResistor));
	memcpy(memory_params->RcompTarget, RcompTarget,
			sizeof(memory_params->RcompTarget));
}

void mainboard_gpio_smi_sleep(void)
{
	int i;

	/* Power down the rails on any sleep type. */
	gpio_t active_high_signals[] = {
		EN_PP3300_KEPLER,
		EN_PP3300_DX_TOUCH,
		EN_PP3300_DX_EMMC,
		EN_PP1800_DX_EMMC,
		EN_PP3300_DX_CAM,
	};

	for (i = 0; i < ARRAY_SIZE(active_high_signals); i++)
		gpio_set(active_high_signals[i], 0);
}
