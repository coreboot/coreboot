/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>

static const struct mb_lpddr4x_cfg malefor_memcfg = {
	/* DQ byte map */
	.dq_map = {
	{  3,  1,  0,  2,  4,  6,  7,  5,   /* Byte 0 */
	  12, 13, 14, 15,  8,  9, 10, 11 }, /* Byte 1 */
	{  0,  7,  1,  6,  2,  4,  3,  5,   /* Byte 2 */
	   8, 15, 14,  9, 13, 10, 12, 11 }, /* Byte 3 */
	{  3,  2,  0,  1,  4,  5,  6,  7,   /* Byte 4 */
	  12, 13, 15, 14,  8,  9, 10, 11 }, /* Byte 5 */
	{  6,  0,  1,  7,  5,  4,  2,  3,   /* Byte 6 */
	  15, 14,  8,  9, 10, 12, 11, 13 }, /* Byte 7 */
	{  5,  0,  1,  3,  4,  2,  7,  6,   /* Byte 0 */
	  11, 14, 13, 12,  8,  9, 15, 10 }, /* Byte 1 */
	{  3,  4,  2,  5,  0,  6,  1,  7,   /* Byte 2 */
	  13, 12, 11, 10, 14, 15,  9,  8 }, /* Byte 3 */
	{  3,  2,  1,  0,  5,  4,  7,  6,   /* Byte 4 */
	  12, 13, 15, 14,  8, 11,  9, 10 }, /* Byte 5 */
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

const struct mb_lpddr4x_cfg *variant_memory_params(void)
{
	return &malefor_memcfg;
}
