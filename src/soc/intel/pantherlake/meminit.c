/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/util.h>
#include <soc/meminit.h>

#define LP5_PHYSICAL_CH_WIDTH		16
#define LP5_CHANNELS			CHANNEL_COUNT(LP5_PHYSICAL_CH_WIDTH)

#define DDR5_PHYSICAL_CH_WIDTH		32
#define DDR5_CHANNELS			CHANNEL_COUNT(DDR5_PHYSICAL_CH_WIDTH)

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

static void meminit_ddr(FSP_M_CONFIG *mem_cfg, const struct mem_ddr_config *ddr_config)
{
	mem_cfg->DqPinsInterleaved = ddr_config->dq_pins_interleaved;
}

static const struct soc_mem_cfg soc_mem_cfg[] = {
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
			/* In mixed topology, channels 2 and 3 are always memory-down. */
			.mixed_topo = BIT(2) | BIT(3),
		},
	},
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

/*
 * DDR5 Memory Controller (MC) Topology:
 *
 * Each DDR5 module consists of two independent 32-bit sub-channels.
 * In this SoC architecture, MC0 and MC1 map to physical channels as follows:
 *
 *       +---------------------------------------------------+
 *       |                     SoC / CPU                     |
 *       |                                                   |
 *       |  +--------------------+   +--------------------+  |
 *       |  |  Memory Controller |   |  Memory Controller |  |
 *       |  |     0 (MC0)        |   |     1 (MC1)        |  |
 *       +--+---------+----------+---+---------+----------+--+
 *                    |                        |
 *          +---------+---------+    +---------+---------+
 *          |                   |    |                   |
 *     +----+-----+       +-----+----+-----+       +-----+----+
 *     | Phys Ch0 |       | Phys Ch1 | Phys Ch2 |       | Phys Ch3 |
 *     +----+-----+       +-----+----+-----+       +-----+----+
 *          |                   |    |                   |
 *          +---------+---------+    +---------+---------+
 *                    |                        |
 *             (Sub-ch A / B)           (Sub-ch A / B)
 *                    |                        |
 *             +------+------+          +------+------+
 *             | DDR5 DIMM 0 |          | DDR5 DIMM 1 |
 *             |  (Slot 0)   |          |  (Slot 1)   |
 *             +-------------+          +-------------+
 *             [ Always Req ]           [ Optional/Pop]
 *
 *
 * Note: Only applicable for DDR5 topologies (LPDDR5/5x use different
 *       channel mappings).
 */
static bool is_ddr5_mc_populated(const struct mem_spd *spd_info, size_t start_ch,
				 size_t end_ch)
{
	if (!spd_info)
		return false;

	for (size_t ch = start_ch; ch <= end_ch; ch++) {
		for (size_t dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			if (spd_info->smbus[ch].addr_dimm[dimm] != 0)
				return true;
		}
	}
	return false;
}

/* Adjusts memory config dynamically at runtime */
static struct soc_mem_cfg get_dynamic_soc_mem_cfg(const struct mb_cfg *mb_cfg,
			 const struct mem_spd *spd_info)
{
	if (mb_cfg->type >= ARRAY_SIZE(soc_mem_cfg))
		die("Unsupported memory type(%d)\n", mb_cfg->type);

	struct soc_mem_cfg cfg = soc_mem_cfg[mb_cfg->type];

	/*
	 * Return the default configuration if:
	 * 1. FSP reads the SPD directly (bypassing coreboot runtime SPD checks).
	 * 2. No SPD data is provided.
	 * 3. Topology is not modular DIMMs (e.g., soldered-down memory).
	 * 4. Memory type is not DDR5 (dynamic channel adjustment only applies to DDR5).
	 */
	if (CONFIG(SOC_INTEL_PANTHERLAKE_FSP_READ_SPD) || !spd_info ||
	    spd_info->topo != MEM_TOPO_DIMM_MODULE || mb_cfg->type != MEM_TYPE_DDR5)
		return cfg;

	bool mc0_present = is_ddr5_mc_populated(spd_info, 0, 1);
	bool mc1_present = is_ddr5_mc_populated(spd_info, 2, 3);

	printk(BIOS_INFO, "DDR5 Runtime Topology: MC0=%s, MC1=%s\n",
	       mc0_present ? "Populated" : "Unpopulated",
	       mc1_present ? "Populated" : "Unpopulated");

	/* Dynamically adjust physical channel masks based on runtime population */
	if (!mc0_present)
		cfg.md_phy_masks.mixed_topo &= ~(BIT(0) | BIT(1));

	if (!mc1_present)
		cfg.md_phy_masks.mixed_topo &= ~(BIT(2) | BIT(3));

	/*
	 * Adjust total channels to scan:
	 * - If neither MC is populated: 0 channels.
	 * - If only MC0 is populated: limit scan to 2 channels (Ch 0 & 1).
	 * - If MC1 is populated: scan all 4 channels (Ch 0-3).
	 */
	if (!mc0_present && !mc1_present)
		cfg.num_phys_channels = 0;
	else if (!mc1_present)
		cfg.num_phys_channels = 2;
	else
		cfg.num_phys_channels = 4;

	return cfg;
}

static void mem_init_spd_upds(FSP_M_CONFIG *mem_cfg, const struct mem_channel_data *data,
				bool expand_channels)
{
	uint64_t *spd_upds[MRC_CHANNELS][CONFIG_DIMMS_PER_CHANNEL] = {
		[0] = { &mem_cfg->MemorySpdPtr000, &mem_cfg->MemorySpdPtr001, },
		[1] = { &mem_cfg->MemorySpdPtr010, &mem_cfg->MemorySpdPtr011, },
		[2] = { &mem_cfg->MemorySpdPtr020, &mem_cfg->MemorySpdPtr021, },
		[3] = { &mem_cfg->MemorySpdPtr030, &mem_cfg->MemorySpdPtr031, },
		[4] = { &mem_cfg->MemorySpdPtr100, &mem_cfg->MemorySpdPtr101, },
		[5] = { &mem_cfg->MemorySpdPtr110, &mem_cfg->MemorySpdPtr111, },
		[6] = { &mem_cfg->MemorySpdPtr120, &mem_cfg->MemorySpdPtr121, },
		[7] = { &mem_cfg->MemorySpdPtr130, &mem_cfg->MemorySpdPtr131, },
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
		bool enable_channel = 0;

		for (dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			uint64_t *spd_ptr = spd_upds[ch][dimm];

			/*
			 * In DDR5 systems, since each DIMM has 2 channels,
			 * we need to copy the SPD data such that:
			 * Channel 0 data is used by channel 0 and 1
			 * Channel 2 data is used by channel 2 and 3
			 * Channel 4 data is used by channel 4 and 5
			 * Channel 6 data is used by channel 6 and 7
			 */
			if (expand_channels)
				*spd_ptr = data->spd[ch & ~1][dimm];
			else
				*spd_ptr = data->spd[ch][dimm];

			if (*spd_ptr)
				enable_channel = 1;
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

	_Static_assert(sizeof(mem_cfg->DqMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH,
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

	_Static_assert(sizeof(mem_cfg->DqsMapCpu2DramMc0Ch0) == CONFIG_MRC_CHANNEL_WIDTH / 8,
		       "Incorrect DQS UPD size!");

	mem_init_dq_dqs_upds(dqs_upds, mb_cfg->dqs_map, upd_size, data, auto_detect);
}

#define CH_DIMM_OFFSET(ch, dimm)        ((ch) * CONFIG_DIMMS_PER_CHANNEL + (dimm))

static void fill_dimm_module_info(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *mb_cfg,
				       const struct mem_spd *spd_info,
				       const struct soc_mem_cfg *dynamic_cfg)
{
	for (size_t ch = 0; ch < dynamic_cfg->num_phys_channels; ch++) {
		for (size_t dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			size_t mrc_ch = dynamic_cfg->phys_to_mrc_map[ch];
			mem_cfg->SpdAddressTable[CH_DIMM_OFFSET(mrc_ch, dimm)] =
				spd_info->smbus[ch].addr_dimm[dimm] << 1;
		}
	}
}

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated)
{
	struct mem_channel_data data;
	bool dq_dqs_auto_detect = false;
	bool expand_channels = false;
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	/* Dynamic Memory Controller Configuration based on runtime population */
	struct soc_mem_cfg dynamic_soc_cfg = get_dynamic_soc_mem_cfg(mb_cfg, spd_info);

	mem_cfg->ECT = mb_cfg->ect;
	mem_cfg->UserBd = mb_cfg->user_bd;
	set_rcomp_config(mem_cfg, mb_cfg);

	switch (mb_cfg->type) {
	case MEM_TYPE_DDR5:
		printk(BIOS_DEBUG, "%s: module type is DDR5\n", __func__);
		meminit_ddr(mem_cfg, &mb_cfg->ddr_config);
		dq_dqs_auto_detect = true;
		expand_channels = true;
		/*
		 * On platforms that rely on FSP-M to probe SPD over SMBus directly
		 * (e.g. Intel RVPs), populate the SpdAddressTable/CkdAddressTable UPDs
		 * so FSP-M handles SMBus reads, and initialize default DQ/DQS swizzle
		 * UPDs before returning early.
		 */
		if (CONFIG(SOC_INTEL_PANTHERLAKE_FSP_READ_SPD) &&
				 (spd_info->topo == MEM_TOPO_DIMM_MODULE)) {
			fill_dimm_module_info(mem_cfg, mb_cfg, spd_info, &dynamic_soc_cfg);
			mem_init_dq_upds(mem_cfg, NULL, mb_cfg, true);
			mem_init_dqs_upds(mem_cfg, NULL, mb_cfg, true);
			return;
		}
		break;
	case MEM_TYPE_LP5X:
		meminit_lp5x(mem_cfg, &mb_cfg->lp5x_config);
		/* For LPCAMM, read SPD using SMBus driver */
		if (spd_info->topo == MEM_TOPO_LP5_CAMM)
			fill_dimm_module_info(mem_cfg, mb_cfg, spd_info, &dynamic_soc_cfg);
		break;
	default:
		die("Unsupported memory type(%d)\n", mb_cfg->type);
	}

	mem_populate_channel_data(memupd, &dynamic_soc_cfg, spd_info,
				  half_populated, &data);
	mem_init_spd_upds(mem_cfg, &data, expand_channels);
	mem_init_dq_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
	mem_init_dqs_upds(mem_cfg, &data, mb_cfg, dq_dqs_auto_detect);
}
