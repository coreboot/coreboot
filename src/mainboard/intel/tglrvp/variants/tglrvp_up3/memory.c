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

#include <arch/cpu.h>
#include <baseboard/variants.h>
#include <intelblocks/mp_init.h>

size_t __weak variant_memory_sku(void)
{
	return 0;
}

static const struct mb_lpddr4x_cfg mem_config = {
	/* DQ byte map */
	.dq_map = {
	{  0,  1,  6,  7,  3,  2,  5,  4,    /* Byte 0 */
	  15,  8,  9, 14, 12, 11, 10, 13 },  /* Byte 1 */
	{ 11, 12,  8, 15,  9, 14, 10, 13,    /* Byte 2 */
	   3,  4,  7,  0,  6,  1,  5,  2 },  /* Byte 3 */
	{  4,  5,  3,  2,  7,  1,  0,  6,    /*	Byte 4 */
	  11, 10, 12, 13,  8,  9, 14, 15 },  /* Byte 5 */
	{ 12, 11,  8, 13, 14, 15,  9, 10,    /* Byte 6 */
	   4,  7,  3,  2,  1,  6,  0,  5 },  /* Byte 7 */
	{ 11, 10,  9,  8, 12, 13, 15, 14,    /* Byte 0 */
	   4,  5,  6,  7,  3,  2,  0,  1 },  /* Byte 1 */
	{  0,  7,  1,  6,  3,  5,  2,  4,    /* Byte 2 */
	   9,  8, 10, 11, 14, 15, 13, 12 },  /* Byte 3 */
	{  4,  5,  6,  1,  3,  2,  7,  0,    /* Byte 4 */
	  10, 13, 12, 11, 14,  9, 15,  8 },  /* Byte 5 */
	{ 10, 12,  9, 15,  8, 11, 13, 14,    /* Byte 6 */
	   3,  4,  1,  2,  6,  0,  5,  7 }   /* Byte 7 */
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
	/* Ch 0     1         2         3  */
	{ 0, 1 }, { 1, 0 }, { 0, 1 }, { 1, 0 },
	{ 1, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 }
	},

	.ect = 1, /* Early Command Training */
};

const struct mb_lpddr4x_cfg *__weak variant_memory_params(void)
{
	return &mem_config;
}
