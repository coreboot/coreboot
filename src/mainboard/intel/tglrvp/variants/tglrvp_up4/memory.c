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
	{  8,  9, 12,  11, 13, 15,  10, 14,      /* Byte 0 */
	   4,  6,  0,   2,  5,  7,   1,  3 },    /* Byte 1 */
	{  2,  3,  0,   6,  1,  7,   5,  4,      /* Byte 2 */
	  15, 14, 13,   8, 12, 11,   9, 10 },    /* Byte 3 */
	{  1,  0,  3,   2,  5,  4,   7,  6,      /* Byte 4 */
	  14, 15, 12,  13,  8, 10,   9, 11 },    /* Byte 5 */
	{  8, 10, 11,   9, 15, 12,  14, 13,      /* Byte 6 */
	   4,  7,  6,   5,  2,  0,  1,  3 },     /* Byte 7 */
	{  8,  9, 10,  11, 13, 12,  15, 14,      /* Byte 0 */
	   7,  6,  4,   5,  0,  2,   1,  3 },    /* Byte 1 */
	{  1,  3,  0,   2,  6,  4,   5,  7,      /* Byte 2 */
	  14, 15, 10,  12,  8, 13,  11,  9 },    /* Byte 3 */
	{  1,  0,  2,   4,  5,  3,   7,  6,      /* Byte 4 */
	  12, 14, 15,  13,  9,  10,  8, 11 },    /* Byte 5 */
	{ 11,  9,  8,  13, 12,  14, 15, 10,      /* Byte 6 */
	   4,  7,  5,   1,  2,   6,  3,  0 }     /* Byte 7 */
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
	/* Ch 0     1         2         3  */
	{ 1, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 },
	{ 1, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 }
	},

	.ect = 1, /* Early Command Training */
};

const struct mb_lpddr4x_cfg *__weak variant_memory_params(void)
{
	return &mem_config;
}
