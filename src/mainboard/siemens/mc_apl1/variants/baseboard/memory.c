/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 * Copyright (C) 2018 Siemens AG
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

#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <compiler.h>
#include <soc/meminit.h>

const struct lpddr4_swizzle_cfg baseboard_lpddr4_swizzle = {
	/* CH0_DQA[0:31] SoC pins -> U22 LPDDR4 module pins */
	.phys[LP4_PHYS_CH0A] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 0, 0, 0, 0, 0, 0, 0, 0 },
	},
	.phys[LP4_PHYS_CH0B] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 0, 0, 0, 0, 0, 0, 0, 0 },
	},
	.phys[LP4_PHYS_CH1A] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 0, 0, 0, 0, 0, 0, 0, 0 },
	},
	.phys[LP4_PHYS_CH1B] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 0, 0, 0, 0, 0, 0, 0, 0 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 0, 0, 0, 0, 0, 0, 0, 0 },
	},
};

const struct lpddr4_swizzle_cfg * __weak variant_lpddr4_swizzle_config(void)
{
	return &baseboard_lpddr4_swizzle;
}
