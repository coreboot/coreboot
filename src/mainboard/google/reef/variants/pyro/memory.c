/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <variant/gpio.h>

static const struct lpddr4_sku skus[] = {
	/*
	 * K4F6E304HB-MGCJ - both logical channels While the parts
	 * are listed at 16Gb there are 2 ranks per channel so indicate
	 * the deneisty as 8Gb per rank.
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
	 * the deneisty as 8Gb per rank.
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
	/* Samsung 290 K4F6E304HB-MGCH 16Gb dual-ch */
	[0xe] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 1,
		.ch1_dual_rank = 1,
		.part_num = "K4F6E304HB-MGCH",
	},
	/* Samsung 280 K4F8E304HB-MGCH 8Gb dual-ch */
	[0xf] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 0,
		.ch1_dual_rank = 0,
		.part_num = "K4F8E304HB-MGCH",
	},
};

static const struct lpddr4_cfg lp4cfg = {
	.skus = skus,
	.num_skus = ARRAY_SIZE(skus),
	.swizzle_config = &baseboard_lpddr4_swizzle,
};

const struct lpddr4_cfg *variant_lpddr4_config(void)
{
	return &lp4cfg;
}
