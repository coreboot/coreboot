/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_lpddr4x_cfg baseboard_memcfg = {
	/* DQ byte map */
	.dq_map = {
	{  0,  1,  2,  3,  4,  5,  6,  7,   /* Byte 0 */
	  12, 13, 14, 15, 11, 10,  9,  8 }, /* Byte 1 */
	{  7,  2,  6,  3,  5,  1,  4,  0,   /* Byte 2 */
	  10,  8,  9, 11, 15, 12, 14, 13 }, /* Byte 3 */
	{  3,  2,  1,  0,  4,  5,  6,  7,   /* Byte 4 */
	  12, 13, 14, 15, 11, 10,  9,  8 }, /* Byte 5 */
	{  7,  0,  1,  6,  5,  4,  2,  3,   /* Byte 6 */
	  15, 14,  8,  9, 10, 12, 11, 13 }, /* Byte 7 */
	{  3,  2,  1,  0,  4,  5,  6,  7,   /* Byte 0 */
	  12, 13, 14, 15, 11, 10,  9,  8 }, /* Byte 1 */
	{  3,  4,  2,  5,  0,  6,  1,  7,   /* Byte 2 */
	  13, 12, 11, 10, 14, 15,  9,  8 }, /* Byte 3 */
	{  3,  2,  1,  0,  7,  4,  5,  6,   /* Byte 4 */
	  15, 14, 13, 12,  8,  9, 10, 11 }, /* Byte 5 */
	{  3,  4,  2,  5,  1,  0,  7,  6,   /* Byte 6 */
	  15, 14,  9,  8, 12, 10, 11, 13 }  /* Byte 7 */
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
	/* Ch 0     1         2         3 */
	{ 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 },
	{ 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }
	},

	.ect = 0, /* Disable Early Command Training */
};

const struct mb_lpddr4x_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
