/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corp.
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
#include <gpio.h>
#include <soc/meminit.h>
#include <variant/gpio.h>

const struct lpddr4_swizzle_cfg baseboard_lpddr4_swizzle = {
	/* CH0_DQA[0:31] SoC pins -> U22 LPDDR4 module pins */
	.phys[LP4_PHYS_CH0A] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 4, 6, 7, 5, 3, 2, 1, 0 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 12, 15, 13, 8, 9, 10, 11, 14 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 17, 18, 19, 16, 23, 20, 21, 22 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 30, 31, 25, 27, 26, 29, 28, 24 },
	},
	.phys[LP4_PHYS_CH0B] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 1, 3, 2, 0, 5, 4, 6, 7 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 15, 14, 13, 12, 8, 9, 11, 10 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 20, 21, 22, 16, 23, 17, 18, 19 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 30, 26, 24, 25, 28, 29, 31, 27 },
	},
	.phys[LP4_PHYS_CH1A] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 15, 14, 13, 12, 8, 9, 10, 11 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 7, 6, 5, 0, 4, 2, 1, 3 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 20, 21, 23, 22, 19, 17, 18, 16 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 24, 27, 26, 30, 25, 31, 28, 29 },
	},
	.phys[LP4_PHYS_CH1B] = {
		/* DQA[0:7] pins of LPDDR4 module. */
		.dqs[LP4_DQS0] = { 0, 4, 7, 1, 6, 5, 3, 2 },
		/* DQA[8:15] pins of LPDDR4 module. */
		.dqs[LP4_DQS1] = { 11, 12, 13, 15, 10, 9, 8, 14 },
		/* DQB[0:7] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS2] = { 19, 21, 17, 16, 22, 23, 18, 20 },
		/* DQB[7:15] pins of LPDDR4 module with offset of 16. */
		.dqs[LP4_DQS3] = { 30, 26, 25, 24, 31, 29, 28, 27 },
	},
};

static const struct lpddr4_sku skus[] = {
	/*
	 * K4F6E304HB-MGCJ - both logical channels While the parts
	 * are listed at 16Gb there are 2 ranks per channel so indicate
	 * the density as 8Gb per rank.
	 */
	[0] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 1,
		.ch1_dual_rank = 1,
		.part_num = "K4F6E304HB-MGCJ",
	},
	/* K4F8E304HB-MGCJ - both logical channels  */
	[1] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.part_num = "K4F8E304HB-MGCJ",
	},
	/*
	 * MT53B512M32D2NP-062WT:C - both logical channels. While the parts
	 * are listed at 16Gb there are 2 ranks per channel so indicate
	 * the density as 8Gb per rank.
	 */
	[2] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 1,
		.ch1_dual_rank = 1,
		.part_num = "MT53B512M32D2NP",
		.disable_periodic_retraining = 1,
	},
	/* MT53B256M32D1NP-062 WT:C - both logical channels */
	[3] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.part_num = "MT53B256M32D1NP",
		.disable_periodic_retraining = 1,
	},
	/*
	 * H9HCNNNBPUMLHR-NLE - both logical channels. While the parts
	 * are listed at 16Gb there are 2 ranks per channel so indicate the
	 * density as 8Gb per rank.
	 */
	[4] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 1,
		.ch1_dual_rank = 1,
		.part_num = "H9HCNNNBPUMLHR",
	},
	/* H9HCNNN8KUMLHR-NLE - both logical channels */
	[5] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.part_num = "H9HCNNN8KUMLHR",
	},
};

static const struct lpddr4_cfg lp4cfg = {
	.skus = skus,
	.num_skus = ARRAY_SIZE(skus),
	.swizzle_config = &baseboard_lpddr4_swizzle,
};

const struct lpddr4_cfg *__attribute__((weak)) variant_lpddr4_config(void)
{
	return &lp4cfg;
}

size_t __attribute__((weak)) variant_memory_sku(void)
{
	gpio_t pads[] = {
		[3] = MEM_CONFIG3, [2] = MEM_CONFIG2,
		[1] = MEM_CONFIG1, [0] = MEM_CONFIG0,
	};

	return gpio_base2_value(pads, ARRAY_SIZE(pads));
}
