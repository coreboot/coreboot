/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <spd_bin.h>
#include <string.h>

static void spd_read_from_cbfs(const struct spd_info *spd_info, uintptr_t *spd_data_ptr,
				size_t *spd_data_len)
{
	size_t spd_index = spd_info->spd_spec.spd_index;

	printk(BIOS_DEBUG, "SPD INDEX = %zu\n", spd_index);

	/* Memory leak is ok since we have memory mapped boot media */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	*spd_data_len = CONFIG_DIMM_SPD_SIZE;
	*spd_data_ptr = spd_cbfs_map(spd_index);
	if (!*spd_data_ptr)
		die("spd.bin not found or incorrect index\n");
}

static void get_spd_data(const struct spd_info *spd_info, uintptr_t *spd_data_ptr,
				size_t *spd_data_len)
{
	if (spd_info->read_type == READ_SPD_MEMPTR) {
		*spd_data_ptr = spd_info->spd_spec.spd_data_ptr_info.spd_data_ptr;
		*spd_data_len = spd_info->spd_spec.spd_data_ptr_info.spd_data_len;
		return;
	}

	if (spd_info->read_type == READ_SPD_CBFS) {
		spd_read_from_cbfs(spd_info, spd_data_ptr, spd_data_len);
		return;
	}

	die("no valid way to read SPD info");
}

static void meminit_dq_dqs_map(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *board_cfg,
				bool half_populated)
{
	memcpy(&mem_cfg->RcompResistor, &board_cfg->rcomp_resistor,
		sizeof(mem_cfg->RcompResistor));

	memcpy(&mem_cfg->RcompTarget, &board_cfg->rcomp_targets,
		sizeof(mem_cfg->RcompTarget));

	memcpy(&mem_cfg->DqByteMapCh0, &board_cfg->dq_map[DDR_CH0],
		sizeof(board_cfg->dq_map[DDR_CH0]));

	memcpy(&mem_cfg->DqsMapCpu2DramCh0, &board_cfg->dqs_map[DDR_CH0],
		sizeof(board_cfg->dqs_map[DDR_CH0]));

	if (half_populated)
		return;

	memcpy(&mem_cfg->DqByteMapCh1, &board_cfg->dq_map[DDR_CH1],
		sizeof(board_cfg->dq_map[DDR_CH1]));

	memcpy(&mem_cfg->DqsMapCpu2DramCh1, &board_cfg->dqs_map[DDR_CH1],
		sizeof(board_cfg->dqs_map[DDR_CH1]));
}

static void meminit_channels(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *board_cfg,
				uintptr_t spd_data_ptr, bool half_populated)
{
	/* Channel 0 */
	mem_cfg->MemorySpdPtr00 = spd_data_ptr;
	mem_cfg->MemorySpdPtr01 = 0;

	if (half_populated) {
		printk(BIOS_INFO, "%s: DRAM half-populated\n", __func__);
		spd_data_ptr = 0;
	}

	/* Channel 1 */
	mem_cfg->MemorySpdPtr10 = spd_data_ptr;
	mem_cfg->MemorySpdPtr11 = 0;

	meminit_dq_dqs_map(mem_cfg, board_cfg, half_populated);
}

/* Initialize onboard memory configurations for lpddr4x */
void memcfg_init(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *board_cfg,
			const struct spd_info *spd_info, bool half_populated)
{

	if (spd_info->read_type == READ_SMBUS) {
		for (int i = 0; i < NUM_DIMM_SLOT; i++)
			mem_cfg->SpdAddressTable[i] = spd_info->spd_spec.spd_smbus_address[i];

		meminit_dq_dqs_map(mem_cfg, board_cfg, half_populated);
	} else {
		uintptr_t spd_data_ptr = 0;
		size_t spd_data_len = 0;
		memset(&mem_cfg->SpdAddressTable, 0, sizeof(mem_cfg->SpdAddressTable));
		get_spd_data(spd_info, &spd_data_ptr, &spd_data_len);
		print_spd_info((unsigned char *)spd_data_ptr);

		mem_cfg->MemorySpdDataLen = spd_data_len;
		meminit_channels(mem_cfg, board_cfg, spd_data_ptr, half_populated);
	}

	/* Early Command Training Enabled */
	mem_cfg->ECT = board_cfg->ect;
	mem_cfg->DqPinsInterleaved = board_cfg->dq_pins_interleaved;
	mem_cfg->CaVrefConfig = board_cfg->vref_ca_config;
	mem_cfg->UserBd = board_cfg->UserBd;
}
