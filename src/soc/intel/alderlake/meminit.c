/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <string.h>

#define LPX_PHYSICAL_CH_WIDTH		16
#define LPX_CHANNELS			CHANNEL_COUNT(LPX_PHYSICAL_CH_WIDTH)

#define DDR4_PHYSICAL_CH_WIDTH		64
#define DDR4_CHANNELS			CHANNEL_COUNT(DDR4_PHYSICAL_CH_WIDTH)

#define DDR5_PHYSICAL_CH_WIDTH		32
#define DDR5_CHANNELS			CHANNEL_COUNT(DDR5_PHYSICAL_CH_WIDTH)

static void set_rcomp_config(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *mb_cfg)
{
	if (mb_cfg->rcomp.resistor != 0)
		mem_cfg->RcompResistor = mb_cfg->rcomp.resistor;

	for (size_t i = 0; i < ARRAY_SIZE(mem_cfg->RcompTarget); i++) {
		if (mb_cfg->rcomp.targets[i] != 0)
			mem_cfg->RcompTarget[i] = mb_cfg->rcomp.targets[i];
	}
}

static void meminit_lp4x(FSP_M_CONFIG *mem_cfg)
{
	mem_cfg->DqPinsInterleaved = 0;
}

static void meminit_lp5x(FSP_M_CONFIG *mem_cfg, const struct mem_lp5x_config *lp5x_config)
{
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->Lp5CccConfig = lp5x_config->ccc_config;
}

static void meminit_ddr(FSP_M_CONFIG *mem_cfg, const struct mem_ddr_config *ddr_config)
{
	mem_cfg->DqPinsInterleaved = ddr_config->dq_pins_interleaved;
}

static const struct soc_mem_cfg soc_mem_cfg[] = {
	[MEM_TYPE_DDR4] = {
		.num_phys_channels = DDR4_CHANNELS,
		.phys_to_mrc_map = {
			[0] = 0,
			[1] = 4,
		},
		.md_phy_masks = {
			/*
			 * Only physical channel 0 is populated in case of half-populated
			 * configuration.
			 */
			.half_channel = BIT(0),
			/* In mixed topologies, channel 1 is always memory-down. */
			.mixed_topo = BIT(1),
		},
	},
	[MEM_TYPE_DDR5] = {
		.num_phys_channels = DDR5_CHANNELS,
		.phys_to_mrc_map = {
			[0] = 0,
			[1] = 1,
			[2] = 4,
			[3] = 5,
		},
		.md_phy_masks = {
			/*
			 * Physical channels 0 and 1 are populated in case of
			 * half-populated configurations.
			 */
			.half_channel = BIT(0) | BIT(1),
			/* In mixed topologies, channels 2 and 3 are always memory-down. */
			.mixed_topo = BIT(2) | BIT(3),
		},
	},
	[MEM_TYPE_LP4X] = {
		.num_phys_channels = LPX_CHANNELS,
		.phys_to_mrc_map = {
			[0] = 0,
			[1] = 1,
			[2] = 2,
			[3] = 3,
			[4] = 4,
			[5] = 5,
			[6] = 6,
			[7] = 7,
		},
		.md_phy_masks = {
			/*
			 * Physical channels 0, 1, 2 and 3 are populated in case of
			 * half-populated configurations.
			 */
			.half_channel = BIT(0) | BIT(1) | BIT(2) | BIT(3),
			/* LP4x does not support mixed topologies. */
		},
	},
	[MEM_TYPE_LP5X] = {
		.num_phys_channels = LPX_CHANNELS,
		.phys_to_mrc_map = {
			[0] = 0,
			[1] = 1,
			[2] = 2,
			[3] = 3,
			[4] = 4,
			[5] = 5,
			[6] = 6,
			[7] = 7,
		},
		.md_phy_masks = {
			/*
			 * Physical channels 0, 1, 2 and 3 are populated in case of
			 * half-populated configurations.
			 */
			.half_channel = BIT(0) | BIT(1) | BIT(2) | BIT(3),
			/* LP5x does not support mixed topologies. */
		},
	},
};

static void mem_init_spd_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data)
{
	uint32_t *spd_upds[MRC_CHANNELS][CONFIG_DIMMS_PER_CHANNEL] = {
		[0] = { &mem_cfg->MemorySpdPtr00, &mem_cfg->MemorySpdPtr01, },
		[1] = { &mem_cfg->MemorySpdPtr02, &mem_cfg->MemorySpdPtr03, },
		[2] = { &mem_cfg->MemorySpdPtr04, &mem_cfg->MemorySpdPtr05, },
		[3] = { &mem_cfg->MemorySpdPtr06, &mem_cfg->MemorySpdPtr07, },
		[4] = { &mem_cfg->MemorySpdPtr08, &mem_cfg->MemorySpdPtr09, },
		[5] = { &mem_cfg->MemorySpdPtr10, &mem_cfg->MemorySpdPtr11, },
		[6] = { &mem_cfg->MemorySpdPtr12, &mem_cfg->MemorySpdPtr13, },
		[7] = { &mem_cfg->MemorySpdPtr14, &mem_cfg->MemorySpdPtr15, },
	};
	uint8_t *disable_dimm_upds[MRC_CHANNELS] = {
		&mem_cfg->DisableDimmMc0Ch0,
		&mem_cfg->DisableDimmMc0Ch1,
		&mem_cfg->DisableDimmMc0Ch2,
		&mem_cfg->DisableDimmMc0Ch3,
		&mem_cfg->DisableDimmMc1Ch0,
		&mem_cfg->DisableDimmMc1Ch1,
		&mem_cfg->DisableDimmMc1Ch2,
		&mem_cfg->DisableDimmMc1Ch3,
	};
	size_t ch, dimm;

	mem_cfg->MemorySpdDataLen = data->spd_len;

	for (ch = 0; ch < MRC_CHANNELS; ch++) {
		uint8_t *disable_dimm_ptr = disable_dimm_upds[ch];
		*disable_dimm_ptr = 0;

		for (dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			uint32_t *spd_ptr = spd_upds[ch][dimm];

			*spd_ptr = data->spd[ch][dimm];
			if (!*spd_ptr)
				*disable_dimm_ptr |= BIT(dimm);
		}
	}
}

static void mem_init_dq_dqs_upds(void *upds[MRC_CHANNELS], const void *map, size_t upd_size,
				const struct mem_channel_data *data, bool auto_detect)
{
	size_t i;

	for (i = 0; i < MRC_CHANNELS; i++, map += upd_size) {
		if (auto_detect ||
			!channel_is_populated(i, MRC_CHANNELS, data->ch_population_flags))
			memset(upds[i], 0, upd_size);
		else
			memcpy(upds[i], map, upd_size);
	}
}

static void mem_init_dq_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
				const struct mb_cfg *mb_cfg, bool auto_detect)
{
	void *dq_upds[MRC_CHANNELS] = {
		&mem_cfg->DqMapCpu2DramCh0,
		&mem_cfg->DqMapCpu2DramCh1,
		&mem_cfg->DqMapCpu2DramCh2,
		&mem_cfg->DqMapCpu2DramCh3,
		&mem_cfg->DqMapCpu2DramCh4,
		&mem_cfg->DqMapCpu2DramCh5,
		&mem_cfg->DqMapCpu2DramCh6,
		&mem_cfg->DqMapCpu2DramCh7,
	};

	const size_t upd_size = sizeof(mem_cfg->DqMapCpu2DramCh0);

	_Static_assert(upd_size == CONFIG_MRC_CHANNEL_WIDTH, "Incorrect DQ UPD size!");

	mem_init_dq_dqs_upds(dq_upds, mb_cfg->dq_map, upd_size, data, auto_detect);
}

static void mem_init_dqs_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
				const struct mb_cfg *mb_cfg, bool auto_detect)
{
	void *dqs_upds[MRC_CHANNELS] = {
		&mem_cfg->DqsMapCpu2DramCh0,
		&mem_cfg->DqsMapCpu2DramCh1,
		&mem_cfg->DqsMapCpu2DramCh2,
		&mem_cfg->DqsMapCpu2DramCh3,
		&mem_cfg->DqsMapCpu2DramCh4,
		&mem_cfg->DqsMapCpu2DramCh5,
		&mem_cfg->DqsMapCpu2DramCh6,
		&mem_cfg->DqsMapCpu2DramCh7,
	};

	const size_t upd_size = sizeof(mem_cfg->DqsMapCpu2DramCh0);

	_Static_assert(upd_size == CONFIG_MRC_CHANNEL_WIDTH / 8, "Incorrect DQS UPD size!");

	mem_init_dq_dqs_upds(dqs_upds, mb_cfg->dqs_map, upd_size, data, auto_detect);
}

void memcfg_init(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated)
{
	struct mem_channel_data data;
	bool dq_dqs_auto_detect = false;

	mem_cfg->ECT = mb_cfg->ect;
	mem_cfg->UserBd = mb_cfg->UserBd;
	set_rcomp_config(mem_cfg, mb_cfg);

	switch (mb_cfg->type) {
	case MEM_TYPE_DDR4:
	case MEM_TYPE_DDR5:
		meminit_ddr(mem_cfg, &mb_cfg->ddr_config);
		dq_dqs_auto_detect = true;
		break;
	case MEM_TYPE_LP4X:
		meminit_lp4x(mem_cfg);
		break;
	case MEM_TYPE_LP5X:
		meminit_lp5x(mem_cfg, &mb_cfg->lp5x_config);
		break;
	default:
		die("Unsupported memory type(%d)\n", mb_cfg->type);
	}

	mem_populate_channel_data(&soc_mem_cfg[mb_cfg->type], spd_info, half_populated, &data);
	mem_init_spd_upds(mem_cfg, &data);
	mem_init_dq_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
	mem_init_dqs_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
}
