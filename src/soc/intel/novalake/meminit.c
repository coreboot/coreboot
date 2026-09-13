/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <string.h>

#define LP5_PHYSICAL_CH_WIDTH		16
#define LP5_CHANNELS			CHANNEL_COUNT(LP5_PHYSICAL_CH_WIDTH)

static void set_rcomp_config(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *mb_cfg)
{
	if (mb_cfg->rcomp.resistor)
		mem_cfg->RcompResistor = mb_cfg->rcomp.resistor;

	for (size_t i = 0; i < ARRAY_SIZE(mem_cfg->RcompTarget); i++)
		if (mb_cfg->rcomp.targets[i])
			mem_cfg->RcompTarget[i] = mb_cfg->rcomp.targets[i];
}

static void meminit_lp5x(FSP_M_CONFIG *mem_cfg, const struct mem_lp5x_config *lp5x_config)
{
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->Lp5CccConfig = lp5x_config->ccc_config;
}

static const struct soc_mem_cfg soc_mem_cfg[] = {
	[MEM_TYPE_LP5X] = {
		.num_phys_channels = LP5_CHANNELS,
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
			 * Physical channels 0, 1, 2 and 3 are populated in case
			 * of half-populated configurations.
			 */
			.half_channel = BIT(0) | BIT(1) | BIT(2) | BIT(3),
			/* LP5x does not support mixed topology. */
		},
	},
};

#define SPD_PTR_IDX(mc, ch, dimm) \
	((mc) * (MRC_CHANNELS / 2) * CONFIG_DIMMS_PER_CHANNEL + \
	(ch) * CONFIG_DIMMS_PER_CHANNEL + \
	(dimm))

static void mem_init_spd_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data)
{
	uint64_t *spd_upds[MRC_CHANNELS][CONFIG_DIMMS_PER_CHANNEL] = {
		[0] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 0, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 0, 1)], },
		[1] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 1, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 1, 1)], },
		[2] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 2, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 2, 1)], },
		[3] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 3, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(0, 3, 1)], },
		[4] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 0, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 0, 1)], },
		[5] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 1, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 1, 1)], },
		[6] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 2, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 2, 1)], },
		[7] = { &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 3, 0)], &mem_cfg->MemorySpdPtr[SPD_PTR_IDX(1, 3, 1)], },
	};
	uint8_t *disable_channel_upds[MRC_CHANNELS] = {
		&mem_cfg->DisableMc0Ch0,
		&mem_cfg->DisableMc0Ch1,
		&mem_cfg->DisableMc0Ch2,
		&mem_cfg->DisableMc0Ch3,
		&mem_cfg->DisableMc1Ch0,
		&mem_cfg->DisableMc1Ch1,
		&mem_cfg->DisableMc1Ch2,
		&mem_cfg->DisableMc1Ch3,
	};
	size_t ch, dimm;

	mem_cfg->MemorySpdDataLen = data->spd_len;

	for (ch = 0; ch < MRC_CHANNELS; ch++) {
		uint8_t *disable_channel_ptr = disable_channel_upds[ch];
		bool enable_channel = false;

		for (dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			uint64_t *spd_ptr = spd_upds[ch][dimm];

			*spd_ptr = data->spd[ch][dimm];
			if (*spd_ptr)
				enable_channel = true;
		}
		*disable_channel_ptr = !enable_channel;
	}
}

static void mem_init_dq_dqs_upds(void *upds[MRC_CHANNELS], const void *map, size_t upd_size,
				 const struct mem_channel_data *data, bool auto_detect)
{
	for (size_t i = 0; i < MRC_CHANNELS; i++, map += upd_size) {
		if (auto_detect || !channel_is_populated(i, MRC_CHANNELS,
							 data->ch_population_flags))
			memset(upds[i], 0, upd_size);
		else
			memcpy(upds[i], map, upd_size);
	}
}

static void mem_init_dq_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
			     const struct mb_cfg *mb_cfg, bool auto_detect)
{
	const size_t upd_size = sizeof(mem_cfg->DqMapCpu2DramMc0Ch0);
	void *dq_upds[MRC_CHANNELS] = {
		&mem_cfg->DqMapCpu2DramMc0Ch0,
		&mem_cfg->DqMapCpu2DramMc0Ch1,
		&mem_cfg->DqMapCpu2DramMc0Ch2,
		&mem_cfg->DqMapCpu2DramMc0Ch3,
		&mem_cfg->DqMapCpu2DramMc1Ch0,
		&mem_cfg->DqMapCpu2DramMc1Ch1,
		&mem_cfg->DqMapCpu2DramMc1Ch2,
		&mem_cfg->DqMapCpu2DramMc1Ch3,
	};

	static_assert(sizeof(mem_cfg->DqMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH,
		      "Incorrect DQ UPD size!");

	mem_init_dq_dqs_upds(dq_upds, mb_cfg->dq_map, upd_size, data, auto_detect);
}

static void mem_init_dqs_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
			      const struct mb_cfg *mb_cfg, bool auto_detect)
{
	const size_t upd_size = sizeof(mem_cfg->DqsMapCpu2DramMc0Ch0);
	void *dqs_upds[MRC_CHANNELS] = {
		&mem_cfg->DqsMapCpu2DramMc0Ch0,
		&mem_cfg->DqsMapCpu2DramMc0Ch1,
		&mem_cfg->DqsMapCpu2DramMc0Ch2,
		&mem_cfg->DqsMapCpu2DramMc0Ch3,
		&mem_cfg->DqsMapCpu2DramMc1Ch0,
		&mem_cfg->DqsMapCpu2DramMc1Ch1,
		&mem_cfg->DqsMapCpu2DramMc1Ch2,
		&mem_cfg->DqsMapCpu2DramMc1Ch3,
	};

	static_assert(sizeof(mem_cfg->DqsMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH / 8,
		      "Incorrect DQS UPD size!");

	mem_init_dq_dqs_upds(dqs_upds, mb_cfg->dqs_map, upd_size, data, auto_detect);
}

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated)
{
	struct mem_channel_data data;
	bool dq_dqs_auto_detect = false;
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	mem_cfg->ECT = mb_cfg->ect;
	mem_cfg->UserBd = mb_cfg->user_bd;
	set_rcomp_config(mem_cfg, mb_cfg);

	if (mb_cfg->type == MEM_TYPE_LP5X)
		meminit_lp5x(mem_cfg, &mb_cfg->lp5x_config);
	else
		die("Unsupported memory type %d\n", mb_cfg->type);

	mem_populate_channel_data(memupd, &soc_mem_cfg[mb_cfg->type], spd_info,
				  half_populated, &data);
	mem_init_spd_upds(mem_cfg, &data);
	mem_init_dq_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
	mem_init_dqs_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
}
