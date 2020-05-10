/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <baseboard/variants.h>
#include <intelblocks/mp_init.h>

size_t __weak variant_memory_sku(void)
{
	return 0;
}

static const struct lpddr4x_cfg mem_config = {
	/* DQ byte map */
	.dq_map = {
		[0] = {
			{ 8,  9, 12,  11, 13, 15,  10, 14, }, /* DDR0_DQ0[7:0] */
			{ 4,  6,  0,   2,  5,  7,   1,  3, }, /* DDR0_DQ1[7:0] */
		},
		[1] = {
			{  2,  3,  0,   6,  1,  7,   5,  4, },  /* DDR1_DQ0[7:0] */
			{ 15, 14, 13,   8, 12, 11,   9, 10, },  /* DDR1_DQ1[7:0] */
		},
		[2] = {
			{  1,  0,  3,   2,  5,  4,   7,  6, }, /* DDR2_DQ0[7:0] */
			{ 14, 15, 12,  13,  8, 10,   9, 11, }, /* DDR2_DQ1[7:0] */
		},
		[3] = {
			{  8, 10, 11,   9, 15, 12,  14, 13, }, /* DDR3_DQ0[7:0] */
			{  4,  7,  6,   5,  2,  0,  1,  3, },  /* DDR3_DQ1[7:0] */
		},
		[4] = {
			{  8,  9, 10,  11, 13, 12,  15, 14, }, /* DDR4_DQ0[7:0] */
			{  7,  6,  4,   5,  0,  2,   1,  3, }, /* DDR4_DQ1[7:0] */
		},
		[5] = {
			{  1,  3,  0,   2,  6,  4,   5,  7, }, /* DDR5_DQ0[7:0] */
			{ 14, 15, 10,  12,  8, 13,  11,  9, }, /* DDR5_DQ1[7:0] */
		},
		[6] = {
			{  1,  0,  2,   4,  5,  3,   7,  6, }, /* DDR6_DQ0[7:0] */
			{ 12, 14, 15,  13,  9,  10,  8, 11, }, /* DDR6_DQ1[7:0] */
		},
		[7] = {
			{ 11,  9,  8,  13, 12,  14, 15, 10, }, /* DDR7_DQ0[7:0] */
			{  4,  7,  5,   1,  2,   6,  3,  0, }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		[0] = { 1, 0 }, /* DDR0_DQS[1:0] */
		[1] = { 0, 1 }, /* DDR1_DQS[1:0] */
		[2] = { 0, 1 }, /* DDR2_DQS[1:0] */
		[3] = { 1, 0 }, /* DDR3_DQS[1:0] */
		[4] = { 1, 0 }, /* DDR4_DQS[1:0] */
		[5] = { 0, 1 }, /* DDR5_DQS[1:0] */
		[6] = { 0, 1 }, /* DDR6_DQS[1:0] */
		[7] = { 1, 0 }, /* DDR7_DQS[1:0] */
	},

	.ect = 1, /* Early Command Training */
};

const struct lpddr4x_cfg *__weak variant_memory_params(void)
{
	return &mem_config;
}
