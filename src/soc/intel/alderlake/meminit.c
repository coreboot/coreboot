/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <spd_bin.h>
#include <string.h>

enum dimm_enable_options {
	ENABLE_BOTH_DIMMS = 0,
	DISABLE_DIMM0 = 1,
	DISABLE_DIMM1 = 2,
	DISABLE_BOTH_DIMMS = 3
};

static void spd_read_from_cbfs(const struct spd_info *spd_info,
			       uintptr_t *spd_data_ptr, size_t *spd_data_len)
{
	struct region_device spd_rdev;
	size_t spd_index = spd_info->spd_spec.spd_index;

	printk(BIOS_DEBUG, "SPD INDEX = %lu\n", spd_index);
	if (get_spd_cbfs_rdev(&spd_rdev, spd_index) < 0)
		die("spd.bin not found or incorrect index\n");

	*spd_data_len = region_device_sz(&spd_rdev);

	/* Memory leak is ok since we have memory mapped boot media */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	*spd_data_ptr = (uintptr_t)rdev_mmap_full(&spd_rdev);
}

static void get_spd_data(const struct spd_info *spd_info,
			 uintptr_t *spd_data_ptr, size_t *spd_data_len)
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
}

static void meminit_dq_dqs_map(FSP_M_CONFIG *mem_cfg,
			       const struct mb_cfg *board_cfg,
			       bool half_populated)
{
	memcpy(&mem_cfg->RcompResistor, &board_cfg->rcomp_resistor,
		sizeof(mem_cfg->RcompResistor));

	memcpy(&mem_cfg->RcompTarget, &board_cfg->rcomp_targets,
		sizeof(mem_cfg->RcompTarget));

	memcpy(&mem_cfg->DqMapCpu2DramCh0, &board_cfg->dq_map[DDR_CH0],
		sizeof(board_cfg->dq_map[DDR_CH0]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh0, &board_cfg->dqs_map[DDR_CH0],
		sizeof(board_cfg->dqs_map[DDR_CH0]));

	memcpy(&mem_cfg->DqMapCpu2DramCh1, &board_cfg->dq_map[DDR_CH1],
		sizeof(board_cfg->dq_map[DDR_CH1]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh1, &board_cfg->dqs_map[DDR_CH1],
		sizeof(board_cfg->dqs_map[DDR_CH1]));

	memcpy(&mem_cfg->DqMapCpu2DramCh2, &board_cfg->dq_map[DDR_CH2],
		sizeof(board_cfg->dq_map[DDR_CH2]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh2, &board_cfg->dqs_map[DDR_CH2],
		sizeof(board_cfg->dqs_map[DDR_CH2]));

	memcpy(&mem_cfg->DqMapCpu2DramCh3, &board_cfg->dq_map[DDR_CH3],
		sizeof(board_cfg->dq_map[DDR_CH3]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh3, &board_cfg->dqs_map[DDR_CH3],
		sizeof(board_cfg->dqs_map[DDR_CH3]));

	if (half_populated)
		return;

	memcpy(&mem_cfg->DqMapCpu2DramCh4, &board_cfg->dq_map[DDR_CH4],
		sizeof(board_cfg->dq_map[DDR_CH4]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh4, &board_cfg->dqs_map[DDR_CH4],
		sizeof(board_cfg->dqs_map[DDR_CH4]));

	memcpy(&mem_cfg->DqMapCpu2DramCh5, &board_cfg->dq_map[DDR_CH5],
		sizeof(board_cfg->dq_map[DDR_CH5]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh5, &board_cfg->dqs_map[DDR_CH5],
		sizeof(board_cfg->dqs_map[DDR_CH5]));

	memcpy(&mem_cfg->DqMapCpu2DramCh6, &board_cfg->dq_map[DDR_CH6],
		sizeof(board_cfg->dq_map[DDR_CH6]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh6, &board_cfg->dqs_map[DDR_CH6],
		sizeof(board_cfg->dqs_map[DDR_CH6]));

	memcpy(&mem_cfg->DqMapCpu2DramCh7, &board_cfg->dq_map[DDR_CH7],
		sizeof(board_cfg->dq_map[DDR_CH7]));
	memcpy(&mem_cfg->DqsMapCpu2DramCh7, &board_cfg->dqs_map[DDR_CH7],
		sizeof(board_cfg->dqs_map[DDR_CH7]));
}

static void meminit_channels(FSP_M_CONFIG *mem_cfg,
				   const struct mb_cfg *board_cfg,
				   uintptr_t spd_data_ptr,
				   bool half_populated)
{
	uint8_t dimm_cfg = DISABLE_DIMM1; /* Use only DIMM0 */

	/* Channel 0 */
	mem_cfg->DisableDimmMc0Ch0 = dimm_cfg;
	mem_cfg->MemorySpdPtr00 = spd_data_ptr;
	mem_cfg->MemorySpdPtr01 = 0;

	/* Channel 1 */
	mem_cfg->DisableDimmMc0Ch1 = dimm_cfg;
	mem_cfg->MemorySpdPtr02 = spd_data_ptr;
	mem_cfg->MemorySpdPtr03 = 0;

	/* Channel 2 */
	mem_cfg->DisableDimmMc0Ch2 = dimm_cfg;
	mem_cfg->MemorySpdPtr04 = spd_data_ptr;
	mem_cfg->MemorySpdPtr05 = 0;

	/* Channel 3 */
	mem_cfg->DisableDimmMc0Ch3 = dimm_cfg;
	mem_cfg->MemorySpdPtr06 = spd_data_ptr;
	mem_cfg->MemorySpdPtr07 = 0;

	if (half_populated) {
		printk(BIOS_INFO, "%s: DRAM half-populated\n", __func__);
		dimm_cfg = DISABLE_BOTH_DIMMS;
		spd_data_ptr = 0;
	}

	/* Channel 4 */
	mem_cfg->DisableDimmMc1Ch0 = dimm_cfg;
	mem_cfg->MemorySpdPtr08 = spd_data_ptr;
	mem_cfg->MemorySpdPtr09 = 0;

	/* Channel 5 */
	mem_cfg->DisableDimmMc1Ch1 = dimm_cfg;
	mem_cfg->MemorySpdPtr10 = spd_data_ptr;
	mem_cfg->MemorySpdPtr11 = 0;

	/* Channel 6 */
	mem_cfg->DisableDimmMc1Ch2 = dimm_cfg;
	mem_cfg->MemorySpdPtr12 = spd_data_ptr;
	mem_cfg->MemorySpdPtr13 = 0;

	/* Channel 7 */
	mem_cfg->DisableDimmMc1Ch3 = dimm_cfg;
	mem_cfg->MemorySpdPtr14 = spd_data_ptr;
	mem_cfg->MemorySpdPtr15 = 0;

	meminit_dq_dqs_map(mem_cfg, board_cfg, half_populated);
}

/* Initialize onboard memory configurations for lpddr4x */
void memcfg_init(FSP_M_CONFIG *mem_cfg,
			   const struct mb_cfg *board_cfg,
			   const struct spd_info *spd_info,
			   bool half_populated)
{
	if (spd_info->read_type == READ_SMBUS) {
		for (int i = 0; i < NUM_DIMM_SLOT; i++)
			mem_cfg->SpdAddressTable[i] = spd_info->spd_spec.spd_smbus_address[i];
		meminit_dq_dqs_map(mem_cfg, board_cfg, half_populated);
	} else {
		size_t spd_data_len = 0;
		uintptr_t spd_data_ptr = 0;

		memset(&mem_cfg->SpdAddressTable, 0, sizeof(mem_cfg->SpdAddressTable));
		get_spd_data(spd_info, &spd_data_ptr, &spd_data_len);

		mem_cfg->MemorySpdDataLen = spd_data_len;
		meminit_channels(mem_cfg, board_cfg, spd_data_ptr, half_populated);
	}

	mem_cfg->ECT = board_cfg->ect;
	mem_cfg->UserBd = board_cfg->UserBd;
}
