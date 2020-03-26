/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <spd_bin.h>
#include <string.h>

/* If memory is half-populated, then upper half of the channels need to be left empty. */
#define LPDDR4X_CHANNEL_UNPOPULATED(ch, half_populated)		\
	((half_populated) && ((ch) >= (LPDDR4X_CHANNELS / 2)))

enum dimm_enable_options {
	ENABLE_BOTH_DIMMS = 0,
	DISABLE_DIMM0 = 1,
	DISABLE_DIMM1 = 2,
	DISABLE_BOTH_DIMMS = 3
};

static uint8_t get_dimm_cfg(uintptr_t dimm0, uintptr_t dimm1)
{
	if (dimm0 && dimm1)
		return ENABLE_BOTH_DIMMS;
	if (!dimm0 && !dimm1)
		return DISABLE_BOTH_DIMMS;
	if (!dimm1)
		return DISABLE_DIMM1;
	if (!dimm0)
		die("Disabling of only dimm0 is not supported!\n");

	return DISABLE_BOTH_DIMMS;
}

static void init_spd_upds(FSP_M_CONFIG *mem_cfg, int channel, uintptr_t spd_dimm0,
			  uintptr_t spd_dimm1)
{
	mem_cfg->Reserved9[channel] = get_dimm_cfg(spd_dimm0, spd_dimm1);

	switch (channel) {
	case 0:
		mem_cfg->MemorySpdPtr00 = spd_dimm0;
		mem_cfg->MemorySpdPtr01 = spd_dimm1;
		break;

	case 1:
		mem_cfg->MemorySpdPtr02 = spd_dimm0;
		mem_cfg->MemorySpdPtr03 = spd_dimm1;
		break;

	case 2:
		mem_cfg->MemorySpdPtr04 = spd_dimm0;
		mem_cfg->MemorySpdPtr05 = spd_dimm1;
		break;

	case 3:
		mem_cfg->MemorySpdPtr06 = spd_dimm0;
		mem_cfg->MemorySpdPtr07 = spd_dimm1;
		break;

	case 4:
		mem_cfg->MemorySpdPtr08 = spd_dimm0;
		mem_cfg->MemorySpdPtr09 = spd_dimm1;
		break;

	case 5:
		mem_cfg->MemorySpdPtr10 = spd_dimm0;
		mem_cfg->MemorySpdPtr11 = spd_dimm1;
		break;

	case 6:
		mem_cfg->MemorySpdPtr12 = spd_dimm0;
		mem_cfg->MemorySpdPtr13 = spd_dimm1;
		break;

	case 7:
		mem_cfg->MemorySpdPtr14 = spd_dimm0;
		mem_cfg->MemorySpdPtr15 = spd_dimm1;
		break;

	default:
		die("Invalid channel: %d\n", channel);
	}
}

static inline void init_spd_upds_empty(FSP_M_CONFIG *mem_cfg, int channel)
{
	init_spd_upds(mem_cfg, channel, 0, 0);
}

static inline void init_spd_upds_dimm0(FSP_M_CONFIG *mem_cfg, int channel, uintptr_t spd_dimm0)
{
	init_spd_upds(mem_cfg, channel, spd_dimm0, 0);
}

static void init_dq_upds(FSP_M_CONFIG *mem_cfg, int byte_pair, const uint8_t *dq_byte0,
			 const uint8_t *dq_byte1)
{
	uint8_t *dq_upd;

	switch (byte_pair) {
	case 0:
		dq_upd = mem_cfg->DqMapCpu2DramCh0;
		break;
	case 1:
		dq_upd = mem_cfg->DqMapCpu2DramCh1;
		break;
	case 2:
		dq_upd = mem_cfg->DqMapCpu2DramCh2;
		break;
	case 3:
		dq_upd = mem_cfg->DqMapCpu2DramCh3;
		break;
	case 4:
		dq_upd = mem_cfg->DqMapCpu2DramCh4;
		break;
	case 5:
		dq_upd = mem_cfg->DqMapCpu2DramCh5;
		break;
	case 6:
		dq_upd = mem_cfg->DqMapCpu2DramCh6;
		break;
	case 7:
		dq_upd = mem_cfg->DqMapCpu2DramCh7;
		break;
	default:
		die("Invalid byte_pair: %d\n", byte_pair);
	}

	if (dq_byte0 && dq_byte1) {
		memcpy(dq_upd, dq_byte0, BITS_PER_BYTE);
		memcpy(dq_upd + BITS_PER_BYTE, dq_byte1, BITS_PER_BYTE);
	} else {
		memset(dq_upd, 0, BITS_PER_BYTE * 2);
	}
}

static inline void init_dq_upds_empty(FSP_M_CONFIG *mem_cfg, int byte_pair)
{
	init_dq_upds(mem_cfg, byte_pair, NULL, NULL);
}

static void init_dqs_upds(FSP_M_CONFIG *mem_cfg, int byte_pair, uint8_t dqs_byte0,
			  uint8_t dqs_byte1)
{
	uint8_t *dqs_upd;

	switch (byte_pair) {
	case 0:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh0;
		break;
	case 1:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh1;
		break;
	case 2:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh2;
		break;
	case 3:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh3;
		break;
	case 4:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh4;
		break;
	case 5:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh5;
		break;
	case 6:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh6;
		break;
	case 7:
		dqs_upd = mem_cfg->DqsMapCpu2DramCh7;
		break;
	default:
		die("Invalid byte_pair: %d\n", byte_pair);
	}

	dqs_upd[0] = dqs_byte0;
	dqs_upd[1] = dqs_byte1;
}

static inline void init_dqs_upds_empty(FSP_M_CONFIG *mem_cfg, int byte_pair)
{
	init_dqs_upds(mem_cfg, byte_pair, 0, 0);
}

static void read_spd_from_cbfs(uint8_t index, uintptr_t *data, size_t *len)
{
	struct region_device spd_rdev;

	printk(BIOS_DEBUG, "SPD INDEX = %u\n", index);
	if (get_spd_cbfs_rdev(&spd_rdev, index) < 0)
		die("spd.bin not found or incorrect index\n");

	/* Memory leak is ok since we have memory mapped boot media */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	*len = region_device_sz(&spd_rdev);
	*data = (uintptr_t)rdev_mmap_full(&spd_rdev);
}

static void read_md_spd(const struct spd_info *info, uintptr_t *data, size_t *len)
{
	if (info->md_spd_loc == SPD_MEMPTR) {
		*data = info->data_ptr;
		*len = info->data_len;
	} else if (info->md_spd_loc == SPD_CBFS) {
		read_spd_from_cbfs(info->cbfs_index, data, len);
	} else {
		die("Not a valid location(%d) for Memory-down SPD!\n", info->md_spd_loc);
	}

	print_spd_info((unsigned char *)data);
}

void meminit_lpddr4x(FSP_M_CONFIG *mem_cfg, const struct lpddr4x_cfg *board_cfg,
			const struct spd_info *info, bool half_populated)

{
	size_t spd_len;
	uintptr_t spd_data;
	int i;

	if (info->topology != MEMORY_DOWN)
		die("LPDDR4x only support memory-down topology.\n");

	/* LPDDR4x does not allow interleaved memory */
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->ECT = board_cfg->ect;
	mem_cfg->MrcSafeConfig = 0x1;

	read_md_spd(info, &spd_data, &spd_len);
	mem_cfg->MemorySpdDataLen = spd_len;

	for (i = 0; i < LPDDR4X_CHANNELS; i++) {
		if (LPDDR4X_CHANNEL_UNPOPULATED(i, half_populated))
			init_spd_upds_empty(mem_cfg, i);
		else
			init_spd_upds_dimm0(mem_cfg, i, spd_data);
	}

	/*
	 * LPDDR4x memory interface has 2 DQs per channel. Each DQ consists of 8 bits (1
	 * byte). However, FSP UPDs for DQ Map expect a DQ pair (i.e. mapping for 2 bytes) in
	 * each UPD.
	 *
	 * Thus, init_dq_upds() needs to be called for dq pair of each channel.
	 * DqMapCpu2DramCh0 --> dq_map[CHAN=0][0-1]
	 * DqMapCpu2DramCh1 --> dq_map[CHAN=1][0-1]
	 * DqMapCpu2DramCh2 --> dq_map[CHAN=2][0-1]
	 * DqMapCpu2DramCh3 --> dq_map[CHAN=3][0-1]
	 * DqMapCpu2DramCh4 --> dq_map[CHAN=4][0-1]
	 * DqMapCpu2DramCh5 --> dq_map[CHAN=5][0-1]
	 * DqMapCpu2DramCh6 --> dq_map[CHAN=6][0-1]
	 * DqMapCpu2DramCh7 --> dq_map[CHAN=7][0-1]
	 */
	for (i = 0; i < LPDDR4X_CHANNELS; i++) {
		if (LPDDR4X_CHANNEL_UNPOPULATED(i, half_populated))
			init_dq_upds_empty(mem_cfg, i);
		else
			init_dq_upds(mem_cfg, i, board_cfg->dq_map[i][0],
				     board_cfg->dq_map[i][1]);
	}

	/*
	 * LPDDR4x memory interface has 2 DQS pairs per channel. FSP UPDs for DQS Map expect a
	 * pair in each UPD.
	 *
	 * Thus, init_dqs_upds() needs to be called for dqs pair of each channel.
	 * DqsMapCpu2DramCh0 --> dqs_map[CHAN=0][0-1]
	 * DqsMapCpu2DramCh1 --> dqs_map[CHAN=1][0-1]
	 * DqsMapCpu2DramCh2 --> dqs_map[CHAN=2][0-1]
	 * DqsMapCpu2DramCh3 --> dqs_map[CHAN=3][0-1]
	 * DqsMapCpu2DramCh4 --> dqs_map[CHAN=4][0-1]
	 * DqsMapCpu2DramCh5 --> dqs_map[CHAN=5][0-1]
	 * DqsMapCpu2DramCh6 --> dqs_map[CHAN=6][0-1]
	 * DqsMapCpu2DramCh7 --> dqs_map[CHAN=7][0-1]
	 */
	for (i = 0; i < LPDDR4X_CHANNELS; i++) {
		if (LPDDR4X_CHANNEL_UNPOPULATED(i, half_populated))
			init_dqs_upds_empty(mem_cfg, i);
		else
			init_dqs_upds(mem_cfg, i, board_cfg->dqs_map[i][0],
				      board_cfg->dqs_map[i][1]);
	}
}
