/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit_tgl.h>
#include <spd_bin.h>
#include <string.h>

enum dimm_enable_options {
	ENABLE_BOTH_DIMMS = 0,
	DISABLE_DIMM0 = 1,
	DISABLE_DIMM1 = 2,
	DISABLE_BOTH_DIMMS = 3
};

#define MEM_INIT_CH_DQ_DQS_MAP(_mem_cfg, _b_cfg, _ch)		\
	do {							\
		memcpy(&_mem_cfg->DqMapCpu2DramCh ## _ch,	\
		       &_b_cfg->dq_map[_ch],			\
			sizeof(_b_cfg->dq_map[_ch]));		\
		memcpy(&_mem_cfg->DqsMapCpu2DramCh ## _ch,	\
			&_b_cfg->dqs_map[_ch],			\
			sizeof(_b_cfg->dqs_map[_ch]));		\
	} while (0)


static void spd_read_from_cbfs(const struct spd_info *spd,
			       uintptr_t *spd_data_ptr, size_t *spd_data_len)
{
	struct region_device spd_rdev;
	size_t spd_index = spd->spd_spec.spd_index;

	printk(BIOS_DEBUG, "SPD INDEX = %lu\n", spd_index);
	if (get_spd_cbfs_rdev(&spd_rdev, spd_index) < 0)
		die("spd.bin not found or incorrect index\n");

	*spd_data_len = region_device_sz(&spd_rdev);

	/* Memory leak is ok since we have memory mapped boot media */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	*spd_data_ptr = (uintptr_t)rdev_mmap_full(&spd_rdev);
}

static void get_spd_data(const struct spd_info *spd,
			 uintptr_t *spd_data_ptr, size_t *spd_data_len)
{
	if (spd->read_type == READ_SPD_MEMPTR) {
		*spd_data_ptr = spd->spd_spec.spd_data_ptr_info.spd_data_ptr;
		*spd_data_len = spd->spd_spec.spd_data_ptr_info.spd_data_len;
		return;
	}

	if (spd->read_type == READ_SPD_CBFS) {
		spd_read_from_cbfs(spd, spd_data_ptr, spd_data_len);
		return;
	}

	die("no valid way to read SPD info");
}

static void meminit_dq_dqs_map(FSP_M_CONFIG *mem_cfg,
			       const struct mb_lpddr4x_cfg *board_cfg,
			       bool half_populated)
{
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 0);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 1);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 2);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 3);

	if (half_populated)
		return;

	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 4);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 5);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 6);
	MEM_INIT_CH_DQ_DQS_MAP(mem_cfg, board_cfg, 7);
}

static void meminit_channels_dimm0(FSP_M_CONFIG *mem_cfg,
				   const struct mb_lpddr4x_cfg *board_cfg,
				   uintptr_t spd_data_ptr,
				   bool half_populated)
{
	uint8_t dimm_cfg = DISABLE_DIMM1; /* Use only DIMM0 */

	/* Channel 0 */
	mem_cfg->Reserved9[0] = dimm_cfg;
	mem_cfg->MemorySpdPtr00 = spd_data_ptr;
	mem_cfg->MemorySpdPtr01 = 0;

	/* Channel 1 */
	mem_cfg->Reserved9[1] = dimm_cfg;
	mem_cfg->MemorySpdPtr02 = spd_data_ptr;
	mem_cfg->MemorySpdPtr03 = 0;

	/* Channel 2 */
	mem_cfg->Reserved9[2] = dimm_cfg;
	mem_cfg->MemorySpdPtr04 = spd_data_ptr;
	mem_cfg->MemorySpdPtr05 = 0;

	/* Channel 3 */
	mem_cfg->Reserved9[3] = dimm_cfg;
	mem_cfg->MemorySpdPtr06 = spd_data_ptr;
	mem_cfg->MemorySpdPtr07 = 0;

	if (half_populated) {
		printk(BIOS_INFO, "%s: DRAM half-populated\n", __func__);
		dimm_cfg = DISABLE_BOTH_DIMMS;
		spd_data_ptr = 0;
	}

	/* Channel 4 */
	mem_cfg->Reserved9[4] = dimm_cfg;
	mem_cfg->MemorySpdPtr08 = spd_data_ptr;
	mem_cfg->MemorySpdPtr09 = 0;

	/* Channel 5 */
	mem_cfg->Reserved9[5] = dimm_cfg;
	mem_cfg->MemorySpdPtr10 = spd_data_ptr;
	mem_cfg->MemorySpdPtr11 = 0;

	/* Channel 6 */
	mem_cfg->Reserved9[6] = dimm_cfg;
	mem_cfg->MemorySpdPtr12 = spd_data_ptr;
	mem_cfg->MemorySpdPtr13 = 0;

	/* Channel 7 */
	mem_cfg->Reserved9[7] = dimm_cfg;
	mem_cfg->MemorySpdPtr14 = spd_data_ptr;
	mem_cfg->MemorySpdPtr15 = 0;

	meminit_dq_dqs_map(mem_cfg, board_cfg, half_populated);
}

/* Initialize onboard memory configurations for lpddr4x */
void meminit_lpddr4x_dimm0(FSP_M_CONFIG *mem_cfg,
			   const struct mb_lpddr4x_cfg *board_cfg,
			   const struct spd_info *spd,
			   bool half_populated)

{
	size_t spd_data_len;
	uintptr_t spd_data_ptr;

	get_spd_data(spd, &spd_data_ptr, &spd_data_len);
	print_spd_info((unsigned char *)spd_data_ptr);

	mem_cfg->MemorySpdDataLen = spd_data_len;
	meminit_channels_dimm0(mem_cfg, board_cfg, spd_data_ptr,
			       half_populated);

	/* LPDDR4 does not allow interleaved memory */
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->ECT = board_cfg->ect;
	mem_cfg->MrcSafeConfig = 0x1;
}
