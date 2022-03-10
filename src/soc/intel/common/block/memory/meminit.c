/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <intelblocks/meminit.h>
#include <commonlib/region.h>
#include <spd_bin.h>
#include <spd_cache.h>
#include <string.h>
#include <types.h>

_Static_assert(CONFIG_MRC_CHANNEL_WIDTH > 0, "MRC channel width must be >0!");
_Static_assert(CONFIG_DATA_BUS_WIDTH > 0, "Data bus width must be >0!");
_Static_assert(CONFIG_DIMMS_PER_CHANNEL > 0, "DIMMS per channel must be >0!");

/*
 * Given mask of channels that are populated, this function returns the flags
 * indicating which half of the channels are populated.
 */
static enum channel_population populated_mask_to_flag(uint32_t pop_mask, size_t max_channels)
{
	uint32_t full_mask = BIT(max_channels) - 1;
	uint32_t bottom_mask = BIT(max_channels / 2) - 1;
	uint32_t top_mask = ~bottom_mask & full_mask;

	if (pop_mask == full_mask)
		return FULLY_POPULATED;
	else if (pop_mask == bottom_mask)
		return BOTTOM_HALF_POPULATED;
	else if (pop_mask == top_mask)
		return TOP_HALF_POPULATED;
	else if (pop_mask == 0)
		return NO_CHANNEL_POPULATED;

	die("Unsupported channel population mask(0x%x)\n", pop_mask);
}

static void read_spd_md(const struct soc_mem_cfg *soc_mem_cfg, const struct mem_spd *info,
			bool half_populated, struct mem_channel_data *channel_data,
			size_t *spd_len)
{
	size_t ch;
	size_t num_phys_ch = soc_mem_cfg->num_phys_channels;
	uintptr_t spd_data;

	/*
	 * For memory down topologies, start with full mask as per the number
	 * of physical channels and mask out any channels based on mixed
	 * topology or half populated flag as set by the mainboard.
	 */
	uint32_t pop_mask = BIT(num_phys_ch) - 1;

	if (!(info->topo & MEM_TOPO_MEMORY_DOWN))
		return;

	if (info->topo == MEM_TOPO_MIXED)
		pop_mask &= soc_mem_cfg->md_phy_masks.mixed_topo;

	if (half_populated)
		pop_mask &= soc_mem_cfg->md_phy_masks.half_channel;

	if (pop_mask == 0)
		die("Memory technology does not support the selected configuration!\n");

	printk(BIOS_DEBUG, "SPD index = %zu\n", info->cbfs_index);

	/* Memory leak is ok as long as we have memory mapped boot media */
	_Static_assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED),
		"Function assumes memory-mapped boot media");

	*spd_len = CONFIG_DIMM_SPD_SIZE;
	spd_data = spd_cbfs_map(info->cbfs_index);
	if (!spd_data)
		die("SPD not found in CBFS or incorrect index!\n");

	print_spd_info((uint8_t *)spd_data);

	for (ch = 0; ch < num_phys_ch; ch++) {
		if (!(pop_mask & BIT(ch)))
			continue;

		int mrc_ch = soc_mem_cfg->phys_to_mrc_map[ch];

		/*
		 * Memory down topology simulates a DIMM. So, the assumption is
		 * that there is a single DIMM per channel when using memory
		 * down topology. As SPD describes a DIMM, only DIMM0 for each
		 * physical channel is filled here.
		 */
		channel_data->spd[mrc_ch][0] = spd_data;
	}

	channel_data->ch_population_flags |= populated_mask_to_flag(pop_mask, num_phys_ch);
}

#define CH_DIMM_OFFSET(ch, dimm)	((ch) * CONFIG_DIMMS_PER_CHANNEL + (dimm))

static bool read_spd_dimm(FSPM_UPD *memupd, const struct soc_mem_cfg *soc_mem_cfg,
			const struct mem_spd *info, bool half_populated,
			struct mem_channel_data *channel_data,
			size_t *spd_len, bool *dimms_changed)
{
	size_t ch, dimm;
	struct spd_block blk = { 0 };
	size_t num_phys_ch = soc_mem_cfg->num_phys_channels;

	/*
	 * For DIMM modules, start with mask set to no channels populated. If
	 * SPD is read successfully from EEPROM for any channel, then that
	 * channel is marked as populated.
	 */
	uint32_t pop_mask = 0;
	*dimms_changed = false;

	if (!(info->topo & MEM_TOPO_DIMM_MODULE))
		return false;

	for (ch = 0; ch < num_phys_ch; ch++) {
		for (dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			blk.addr_map[CH_DIMM_OFFSET(ch, dimm)] =
				info->smbus[ch].addr_dimm[dimm];
		}
	}

	if (CONFIG(SPD_CACHE_ENABLE)) {
		uint8_t *spd_cache;
		size_t spd_cache_sz;
		bool need_update_cache = false;
		bool dimm_changed = true;

		/* load spd cache from RW_SPD_CACHE */
		if (load_spd_cache(&spd_cache, &spd_cache_sz) == CB_SUCCESS) {
			if (!spd_cache_is_valid(spd_cache, spd_cache_sz)) {
				printk(BIOS_WARNING, "Invalid SPD cache\n");
			} else {
				dimm_changed = check_if_dimm_changed(spd_cache, &blk);
				if (dimm_changed) {
					/*
					 * Set flag to indicate that the
					 * mrc_cache need to be invalidated
					 */
					printk(BIOS_INFO,
					"DIMM change, invalidate cache.\n");
					*dimms_changed = true;
				}
			}
			need_update_cache = true;
		}

		if (!dimm_changed) {
			printk(BIOS_INFO, "Use the SPD cache data\n");
			spd_fill_from_cache(spd_cache, &blk);
		} else {
			/* Access memory info through SMBUS. */
			get_spd_smbus(&blk);

			if (need_update_cache &&
				update_spd_cache(&blk) == CB_ERR)
				printk(BIOS_ERR, "update SPD cache failed\n");
		}
	} else {
		get_spd_smbus(&blk);
	}

	*spd_len = blk.len;

	for (ch = 0; ch < num_phys_ch; ch++) {
		size_t mrc_ch = soc_mem_cfg->phys_to_mrc_map[ch];

		for (dimm = 0; dimm < CONFIG_DIMMS_PER_CHANNEL; dimm++) {
			uint8_t *spd_data = blk.spd_array[CH_DIMM_OFFSET(ch, dimm)];
			if (spd_data == NULL)
				continue;

			print_spd_info(spd_data);

			channel_data->spd[mrc_ch][dimm] = (uintptr_t)(void *)spd_data;
			pop_mask |= BIT(ch);
		}
	}

	channel_data->ch_population_flags |= populated_mask_to_flag(pop_mask, num_phys_ch);

	return pop_mask != 0;
}

void mem_populate_channel_data(FSPM_UPD *memupd, const struct soc_mem_cfg *soc_mem_cfg,
				const struct mem_spd *spd_info,
				bool half_populated,
				struct mem_channel_data *data,
				bool *dimms_changed)
{
	size_t spd_md_len = 0, spd_dimm_len = 0;
	bool have_dimms;

	memset(data, 0, sizeof(*data));

	read_spd_md(soc_mem_cfg, spd_info, half_populated, data, &spd_md_len);
	have_dimms = read_spd_dimm(memupd, soc_mem_cfg, spd_info, half_populated, data,
				&spd_dimm_len, dimms_changed);

	if (data->ch_population_flags == NO_CHANNEL_POPULATED)
		die("No channels are populated. Incorrect memory configuration!\n");

	if (spd_info->topo == MEM_TOPO_MEMORY_DOWN) {
		data->spd_len = spd_md_len;
	} else if (spd_info->topo == MEM_TOPO_DIMM_MODULE) {
		data->spd_len = spd_dimm_len;
	} else {
		/*
		 * SPD lengths must match for CBFS and EEPROM SPD for mixed
		 * topology. Skip this check when no DIMMs are installed.
		 */
		if (have_dimms && spd_md_len != spd_dimm_len)
			die("Length of SPD does not match for mixed topology!\n");

		/* Use memory-down SPD length in case there are no DIMMs installed */
		data->spd_len = spd_md_len;
	}
}
