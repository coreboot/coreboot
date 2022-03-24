/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <string.h>

#define LP4X_CH_WIDTH			16
#define LP4X_CHANNELS			CHANNEL_COUNT(LP4X_CH_WIDTH)

#define DDR4_CH_WIDTH			64
#define DDR4_CHANNELS			CHANNEL_COUNT(DDR4_CH_WIDTH)

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
			/* In mixed topologies, channel 0 is always memory-down. */
			.mixed_topo = BIT(0),
		},
	},
	[MEM_TYPE_LP4X] = {
		.num_phys_channels = LP4X_CHANNELS,
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
};

static void mem_init_spd_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data)
{
	uint32_t *spd_upds[MRC_CHANNELS][CONFIG_DIMMS_PER_CHANNEL] = {
		[0] = { &mem_cfg->MemorySpdPtr000, &mem_cfg->MemorySpdPtr001, },
		[1] = { &mem_cfg->MemorySpdPtr010, &mem_cfg->MemorySpdPtr011, },
		[2] = { &mem_cfg->MemorySpdPtr020, &mem_cfg->MemorySpdPtr021, },
		[3] = { &mem_cfg->MemorySpdPtr030, &mem_cfg->MemorySpdPtr031, },
		[4] = { &mem_cfg->MemorySpdPtr100, &mem_cfg->MemorySpdPtr101, },
		[5] = { &mem_cfg->MemorySpdPtr110, &mem_cfg->MemorySpdPtr111, },
		[6] = { &mem_cfg->MemorySpdPtr120, &mem_cfg->MemorySpdPtr121, },
		[7] = { &mem_cfg->MemorySpdPtr130, &mem_cfg->MemorySpdPtr131, },
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
	int ch, dimm;

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
				const struct mem_channel_data *data)
{
	size_t i;

	for (i = 0; i < MRC_CHANNELS; i++, map += upd_size) {
		if (channel_is_populated(i, MRC_CHANNELS, data->ch_population_flags))
			memcpy(upds[i], map, upd_size);
		else
			memset(upds[i], 0, upd_size);
	}
}

static void mem_init_dq_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
				const struct mb_cfg *mb_cfg)
{
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

	const size_t upd_size = sizeof(mem_cfg->DqMapCpu2DramMc0Ch0);

	_Static_assert(sizeof(mem_cfg->DqMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH,
		       "Incorrect DQ UPD size!");

	mem_init_dq_dqs_upds(dq_upds, mb_cfg->dq_map, upd_size, data);
}

static void mem_init_dqs_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
				const struct mb_cfg *mb_cfg)
{
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

	const size_t upd_size = sizeof(mem_cfg->DqsMapCpu2DramMc0Ch0);

	_Static_assert(sizeof(mem_cfg->DqsMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH / 8,
		       "Incorrect DQS UPD size!");

	mem_init_dq_dqs_upds(dqs_upds, mb_cfg->dqs_map, upd_size, data);
}

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated)
{
	struct mem_channel_data data;
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	if (mb_cfg->type >= ARRAY_SIZE(soc_mem_cfg))
		die("Invalid memory type(%x)!\n", mb_cfg->type);

	mem_populate_channel_data(memupd, &soc_mem_cfg[mb_cfg->type], spd_info, half_populated,
			&data);
	mem_init_spd_upds(mem_cfg, &data);
	mem_init_dq_upds(mem_cfg, &data, mb_cfg);
	mem_init_dqs_upds(mem_cfg, &data, mb_cfg);

	mem_cfg->ECT = mb_cfg->ect;

	switch (mb_cfg->type) {
	case MEM_TYPE_DDR4:
		mem_cfg->DqPinsInterleaved = mb_cfg->ddr4_config.dq_pins_interleaved;
		break;
	case MEM_TYPE_LP4X:
		/* LPDDR4x does not allow interleaved memory */
		mem_cfg->DqPinsInterleaved = 0;
		break;
	default:
		die("Unsupported memory type(%d)\n", mb_cfg->type);
	}

}
