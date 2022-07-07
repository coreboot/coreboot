/* SPDX-License-Identifier: GPL-2.0-only */
#include <console/console.h>
#include <fsp/util.h>
#include <memory_info.h>
#include <soc/meminit.h>
#include <stddef.h>
#include <fsp/soc_binding.h>
#include <string.h>

static size_t memory_size_mib;

size_t memory_in_system_in_mib(void)
{
	return memory_size_mib;
}

static void accumulate_channel_memory(int density, int dual_rank)
{
	/* For this platform LPDDR4 memory is 4 DRAM parts that are x32. 2 of
	   the parts are composed into a x64 memory channel. Thus there are 2
	   channels composed of 2 DRAMs. */
	size_t sz = density;

	/* Two DRAMs per channel. */
	sz *= 2;

	/* Two ranks per channel. */
	if (dual_rank)
		sz *= 2;

	sz *= GiB / MiB;

	memory_size_mib += sz;
}

size_t iohole_in_mib(void)
{
	return 2 * (GiB / MiB);
}

static void set_lpddr4_defaults(FSP_M_CONFIG *cfg)
{
	uint8_t odt_config;

	/* Enable memory down BGA since it's the only LPDDR4 packaging. */
	cfg->Package = 1;
	cfg->MemoryDown = 1;

	cfg->ScramblerSupport = 1;
	cfg->ChannelHashMask = 0x36;
	cfg->SliceHashMask = 0x9;
	cfg->InterleavedMode = 2;
	cfg->ChannelsSlicesEnable = 0;
	cfg->MinRefRate2xEnable = 0;
	cfg->DualRankSupportEnable = 1;
	/* Don't enforce a memory size limit. */
	cfg->MemorySizeLimit = 0;
	/* Field is in MiB units. */
	cfg->LowMemoryMaxValue = iohole_in_mib();
	/* No restrictions on memory above 4GiB */
	cfg->HighMemoryMaxValue = 0;

	/* Always default to attempt to use saved training data. */
	cfg->DisableFastBoot = 0;

	/* LPDDR4 is memory down so no SPD addresses. */
	cfg->DIMM0SPDAddress = 0;
	cfg->DIMM1SPDAddress = 0;

	/* Clear all the rank enables. */
	cfg->Ch0_RankEnable = 0x0;
	cfg->Ch1_RankEnable = 0x0;
	cfg->Ch2_RankEnable = 0x0;
	cfg->Ch3_RankEnable = 0x0;

	/*
	 * Set the device width to x16 which is half a LPDDR4 module as that's
	 * what the reference code expects.
	 */
	cfg->Ch0_DeviceWidth = 0x1;
	cfg->Ch1_DeviceWidth = 0x1;
	cfg->Ch2_DeviceWidth = 0x1;
	cfg->Ch3_DeviceWidth = 0x1;

	/*
	 * Enable bank hashing (bit 1) and rank interleaving (bit 0) with
	 * a 1KiB address mapping (bits 5:4).
	 */
	cfg->Ch0_Option = 0x3;
	cfg->Ch1_Option = 0x3;
	cfg->Ch2_Option = 0x3;
	cfg->Ch3_Option = 0x3;

	/* Set CA ODT with default setting of ODT pins of LPDDR4 modules pulled
	   up to 1.1V. */
	odt_config = ODT_A_B_HIGH_HIGH;

	cfg->Ch0_OdtConfig = odt_config;
	cfg->Ch1_OdtConfig = odt_config;
	cfg->Ch2_OdtConfig = odt_config;
	cfg->Ch3_OdtConfig = odt_config;
}

struct speed_mapping {
	int logical;
	int fsp_value;
};

struct fsp_speed_profiles {
	const struct speed_mapping *mappings;
	size_t num_mappings;
};

static const struct speed_mapping apl_mappings[] = {
	{ .logical = LP4_SPEED_1600, .fsp_value = 0x9 },
	{ .logical = LP4_SPEED_2133, .fsp_value = 0xa },
	{ .logical = LP4_SPEED_2400, .fsp_value = 0xb },
};

static const struct fsp_speed_profiles apl_profile = {
	.mappings = apl_mappings,
	.num_mappings = ARRAY_SIZE(apl_mappings),
};

static const struct speed_mapping glk_mappings[] = {
	{ .logical = LP4_SPEED_1600, .fsp_value = 0x4 },
	{ .logical = LP4_SPEED_2133, .fsp_value = 0x6 },
	{ .logical = LP4_SPEED_2400, .fsp_value = 0x7 },
};

static const struct fsp_speed_profiles glk_profile = {
	.mappings = glk_mappings,
	.num_mappings = ARRAY_SIZE(glk_mappings),
};

static const struct fsp_speed_profiles *get_fsp_profile(void)
{
	if (CONFIG(SOC_INTEL_GEMINILAKE))
		return &glk_profile;
	else
		return &apl_profile;
}

static int validate_speed(int speed)
{
	const struct fsp_speed_profiles *fsp_profile = get_fsp_profile();
	size_t i;

	for (i = 0; i < fsp_profile->num_mappings; i++) {
		/* Mapping exists. */
		if (fsp_profile->mappings[i].logical == speed)
			return speed;
	}

	printk(BIOS_WARNING, "Invalid LPDDR4 speed: %d\n", speed);
	/* Default to slowest speed */
	return LP4_SPEED_1600;
}

static int fsp_memory_profile(int speed)
{
	const struct fsp_speed_profiles *fsp_profile = get_fsp_profile();
	size_t i;

	for (i = 0; i < fsp_profile->num_mappings; i++) {
		if (fsp_profile->mappings[i].logical == speed)
			return fsp_profile->mappings[i].fsp_value;
	}

	/* should never happen. */
	return -1;
}

void meminit_lpddr4(FSP_M_CONFIG *cfg, int speed)
{
	speed = validate_speed(speed);

	printk(BIOS_INFO, "LP4DDR speed is %dMHz\n", speed);
	cfg->Profile = fsp_memory_profile(speed);

	set_lpddr4_defaults(cfg);
}

static void enable_logical_chan0(FSP_M_CONFIG *cfg,
					int rank_density, int dual_rank,
					const struct lpddr4_swizzle_cfg *scfg)
{
	const struct lpddr4_chan_swizzle_cfg *chan;
	/* Number of bytes to copy per DQS. */
	const size_t sz = DQ_BITS_PER_DQS;
	int rank_mask;

	/*
	 * Logical channel 0 is comprised of physical channel 0 and 1.
	 * Physical channel 0 is comprised of the CH0_DQB signals.
	 * Physical channel 1 is comprised of the CH0_DQA signals.
	 */
	cfg->Ch0_DramDensity = rank_density;
	cfg->Ch1_DramDensity = rank_density;
	/* Enable ranks on both channels depending on dual rank option. */
	rank_mask = dual_rank ? 0x3 : 0x1;
	cfg->Ch0_RankEnable = rank_mask;
	cfg->Ch1_RankEnable = rank_mask;

	/*
	 * CH0_DQB byte lanes in the bit swizzle configuration field are
	 * not 1:1. The mapping within the swizzling field is:
	 *   indices [0:7]   - byte lane 1 (DQS1) DQ[8:15]
	 *   indices [8:15]  - byte lane 0 (DQS0) DQ[0:7]
	 *   indices [16:23] - byte lane 3 (DQS3) DQ[24:31]
	 *   indices [24:31] - byte lane 2 (DQS2) DQ[16:23]
	 */
	chan = &scfg->phys[LP4_PHYS_CH0B];
	memcpy(&cfg->Ch0_Bit_swizzling[0], &chan->dqs[LP4_DQS1], sz);
	memcpy(&cfg->Ch0_Bit_swizzling[8], &chan->dqs[LP4_DQS0], sz);
	memcpy(&cfg->Ch0_Bit_swizzling[16], &chan->dqs[LP4_DQS3], sz);
	memcpy(&cfg->Ch0_Bit_swizzling[24], &chan->dqs[LP4_DQS2], sz);

	/*
	 * CH0_DQA byte lanes in the bit swizzle configuration field are 1:1.
	 */
	chan = &scfg->phys[LP4_PHYS_CH0A];
	memcpy(&cfg->Ch1_Bit_swizzling[0], &chan->dqs[LP4_DQS0], sz);
	memcpy(&cfg->Ch1_Bit_swizzling[8], &chan->dqs[LP4_DQS1], sz);
	memcpy(&cfg->Ch1_Bit_swizzling[16], &chan->dqs[LP4_DQS2], sz);
	memcpy(&cfg->Ch1_Bit_swizzling[24], &chan->dqs[LP4_DQS3], sz);
}

static void enable_logical_chan1(FSP_M_CONFIG *cfg,
					int rank_density, int dual_rank,
					const struct lpddr4_swizzle_cfg *scfg)
{
	const struct lpddr4_chan_swizzle_cfg *chan;
	/* Number of bytes to copy per DQS. */
	const size_t sz = DQ_BITS_PER_DQS;
	int rank_mask;

	/*
	 * Logical channel 1 is comprised of physical channel 2 and 3.
	 * Physical channel 2 is comprised of the CH1_DQB signals.
	 * Physical channel 3 is comprised of the CH1_DQA signals.
	 */
	cfg->Ch2_DramDensity = rank_density;
	cfg->Ch3_DramDensity = rank_density;
	/* Enable ranks on both channels depending on dual rank option. */
	rank_mask = dual_rank ? 0x3 : 0x1;
	cfg->Ch2_RankEnable = rank_mask;
	cfg->Ch3_RankEnable = rank_mask;

	/*
	 * CH1_DQB byte lanes in the bit swizzle configuration field are
	 * not 1:1. The mapping within the swizzling field is:
	 *   indices [0:7]   - byte lane 1 (DQS1) DQ[8:15]
	 *   indices [8:15]  - byte lane 0 (DQS0) DQ[0:7]
	 *   indices [16:23] - byte lane 3 (DQS3) DQ[24:31]
	 *   indices [24:31] - byte lane 2 (DQS2) DQ[16:23]
	 */
	chan = &scfg->phys[LP4_PHYS_CH1B];
	memcpy(&cfg->Ch2_Bit_swizzling[0], &chan->dqs[LP4_DQS1], sz);
	memcpy(&cfg->Ch2_Bit_swizzling[8], &chan->dqs[LP4_DQS0], sz);
	memcpy(&cfg->Ch2_Bit_swizzling[16], &chan->dqs[LP4_DQS3], sz);
	memcpy(&cfg->Ch2_Bit_swizzling[24], &chan->dqs[LP4_DQS2], sz);

	/*
	 * CH1_DQA byte lanes in the bit swizzle configuration field are 1:1.
	 */
	chan = &scfg->phys[LP4_PHYS_CH1A];
	memcpy(&cfg->Ch3_Bit_swizzling[0], &chan->dqs[LP4_DQS0], sz);
	memcpy(&cfg->Ch3_Bit_swizzling[8], &chan->dqs[LP4_DQS1], sz);
	memcpy(&cfg->Ch3_Bit_swizzling[16], &chan->dqs[LP4_DQS2], sz);
	memcpy(&cfg->Ch3_Bit_swizzling[24], &chan->dqs[LP4_DQS3], sz);
}

void meminit_lpddr4_enable_channel(FSP_M_CONFIG *cfg, int logical_chan,
					int rank_density_gb, int dual_rank,
					const struct lpddr4_swizzle_cfg *scfg)
{
	int fsp_rank_density;

	switch (rank_density_gb) {
	case LP4_4Gb_DENSITY:
		fsp_rank_density = 0;
		break;
	case LP4_6Gb_DENSITY:
		fsp_rank_density = 1;
		break;
	case LP4_8Gb_DENSITY:
		fsp_rank_density = 2;
		break;
	case LP4_12Gb_DENSITY:
		fsp_rank_density = 3;
		break;
	case LP4_16Gb_DENSITY:
		fsp_rank_density = 4;
		break;
	default:
		printk(BIOS_ERR, "Invalid LPDDR4 density: %d Gb\n", rank_density_gb);
		return;
	}

	switch (logical_chan) {
	case LP4_LCH0:
		enable_logical_chan0(cfg, fsp_rank_density, dual_rank, scfg);
		break;
	case LP4_LCH1:
		enable_logical_chan1(cfg, fsp_rank_density, dual_rank, scfg);
		break;
	default:
		printk(BIOS_ERR, "Invalid logical channel: %d\n", logical_chan);
		return;
	}
	accumulate_channel_memory(rank_density_gb, dual_rank);
}

void meminit_lpddr4_by_sku(FSP_M_CONFIG *cfg,
				const struct lpddr4_cfg *lpcfg, size_t sku_id)
{
	const struct lpddr4_sku *sku;

	if (sku_id >= lpcfg->num_skus) {
		printk(BIOS_ERR, "Too few LPDDR4 SKUs: 0x%zx/0x%zx\n",
			sku_id, lpcfg->num_skus);
		return;
	}

	printk(BIOS_INFO, "LPDDR4 SKU id = 0x%zx\n", sku_id);

	sku = &lpcfg->skus[sku_id];

	meminit_lpddr4(cfg, sku->speed);

	if (sku->ch0_rank_density) {
		printk(BIOS_INFO, "LPDDR4 Ch0 density = %d Gb\n",
			sku->ch0_rank_density);
		meminit_lpddr4_enable_channel(cfg, LP4_LCH0,
						sku->ch0_rank_density,
						sku->ch0_dual_rank,
						lpcfg->swizzle_config);
	}

	if (sku->ch1_rank_density) {
		printk(BIOS_INFO, "LPDDR4 Ch1 density = %d Gb\n",
			sku->ch1_rank_density);
		meminit_lpddr4_enable_channel(cfg, LP4_LCH1,
						sku->ch1_rank_density,
						sku->ch1_dual_rank,
						lpcfg->swizzle_config);
	}

	cfg->PeriodicRetrainingDisable = sku->disable_periodic_retraining;
}

uint8_t fsp_memory_soc_version(void)
{
	/* Bump this value when the memory configuration parameters change. */
	return 1;
}
