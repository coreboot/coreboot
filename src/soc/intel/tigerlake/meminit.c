/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/meminit.h>
#include <spd_bin.h>
#include <string.h>

/* If memory is half-populated, then upper half of the channels need to be left empty. */
#define LPDDR4X_CHANNEL_UNPOPULATED(ch, half_populated)		\
	((half_populated) && ((ch) >= (LPDDR4X_CHANNELS / 2)))

/*
 * Translate DDR4 channel # to FSP UPD index # for the channel.
 * Channel 0 -> Index 0
 * Channel 1 -> Index 4
 * Index 1-3 and 5-7 are unused.
 */
#define DDR4_FSP_UPD_CHANNEL_IDX(x)		((x) * 4)

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
	uint8_t dimm_cfg = get_dimm_cfg(spd_dimm0, spd_dimm1);

	switch (channel) {
	case 0:
		mem_cfg->DisableDimmCh0 = dimm_cfg;
		mem_cfg->MemorySpdPtr00 = spd_dimm0;
		mem_cfg->MemorySpdPtr01 = spd_dimm1;
		break;

	case 1:
		mem_cfg->DisableDimmCh1 = dimm_cfg;
		mem_cfg->MemorySpdPtr02 = spd_dimm0;
		mem_cfg->MemorySpdPtr03 = spd_dimm1;
		break;

	case 2:
		mem_cfg->DisableDimmCh2 = dimm_cfg;
		mem_cfg->MemorySpdPtr04 = spd_dimm0;
		mem_cfg->MemorySpdPtr05 = spd_dimm1;
		break;

	case 3:
		mem_cfg->DisableDimmCh3 = dimm_cfg;
		mem_cfg->MemorySpdPtr06 = spd_dimm0;
		mem_cfg->MemorySpdPtr07 = spd_dimm1;
		break;

	case 4:
		mem_cfg->DisableDimmCh4 = dimm_cfg;
		mem_cfg->MemorySpdPtr08 = spd_dimm0;
		mem_cfg->MemorySpdPtr09 = spd_dimm1;
		break;

	case 5:
		mem_cfg->DisableDimmCh5 = dimm_cfg;
		mem_cfg->MemorySpdPtr10 = spd_dimm0;
		mem_cfg->MemorySpdPtr11 = spd_dimm1;
		break;

	case 6:
		mem_cfg->DisableDimmCh6 = dimm_cfg;
		mem_cfg->MemorySpdPtr12 = spd_dimm0;
		mem_cfg->MemorySpdPtr13 = spd_dimm1;
		break;

	case 7:
		mem_cfg->DisableDimmCh7 = dimm_cfg;
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

	print_spd_info((uint8_t *) *data);
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

static void read_sodimm_spd(const struct spd_info *info, struct spd_block *blk)
{
	unsigned int i;

	blk->addr_map[0] = info->smbus_info[0].addr_dimm0;
	blk->addr_map[1] = info->smbus_info[0].addr_dimm1;
	blk->addr_map[2] = info->smbus_info[1].addr_dimm0;
	blk->addr_map[3] = info->smbus_info[1].addr_dimm1;

	get_spd_smbus(blk);

	/*
	 * SPD gets printed only if:
	 * a) mainboard provides a non-zero SMBus address and
	 * b) SPD is successfully read using the SMBus address
	 */
	for (i = 0; i < ARRAY_SIZE(blk->addr_map); i++) {
		if (blk->spd_array[i] != NULL)
			print_spd_info((uint8_t *)blk->spd_array[i]);
	}
}

static void ddr4_get_spd(unsigned int channel, const uintptr_t *spd_md_data,
			 const struct spd_block *spd_sodimm_blk,
			 const struct spd_info *info,
			 const bool half_populated, uintptr_t *spd_dimm0,
			 uintptr_t *spd_dimm1)
{
	if (channel == 0) {
		/* For mixed topology, channel 0 can only be Memory_Down */
		if ((info->topology == MEMORY_DOWN) || (info->topology == MIXED)) {
			*spd_dimm0 = *spd_md_data;
			*spd_dimm1 = 0;
		} else if (info->topology == SODIMM) {
			*spd_dimm0 = (uintptr_t)spd_sodimm_blk->spd_array[0];
			*spd_dimm1 = (uintptr_t)spd_sodimm_blk->spd_array[1];
		} else
			die("Undefined memory topology on Channel 0.\n");
	} else if (channel == 1) {
		if (half_populated) {
			*spd_dimm0 = *spd_dimm1 = 0;
		} else if (info->topology == MEMORY_DOWN) {
			*spd_dimm0 = *spd_md_data;
			*spd_dimm1 = 0;
		/* For mixed topology, channel 1 can only be SODIMM */
		} else if ((info->topology == SODIMM) || (info->topology == MIXED)) {
			*spd_dimm0 = (uintptr_t)spd_sodimm_blk->spd_array[2];
			*spd_dimm1 = (uintptr_t)spd_sodimm_blk->spd_array[3];
		} else
			die("Undefined memory topology on channel 1.\n");
	} else
		die("Unsupported channels.\n");
}

/* Initialize DDR4 memory configurations */
void meminit_ddr4(FSP_M_CONFIG *mem_cfg, const struct mb_ddr4_cfg *board_cfg,
			  const struct spd_info *info, const bool half_populated)
{
	uintptr_t spd_md_data;
	size_t spd_md_len;
	uintptr_t spd_dimm0 = 0;
	uintptr_t spd_dimm1 = 0;
	struct spd_block spd_sodimm_blk;
	unsigned int i;
	unsigned int index = 0;

	/* Early Command Training Enabled */
	mem_cfg->ECT = board_cfg->ect;
	mem_cfg->DqPinsInterleaved = board_cfg->dq_pins_interleaved;

	if ((info->topology == MEMORY_DOWN) || (info->topology == MIXED)) {
		read_md_spd(info, &spd_md_data, &spd_md_len);
		mem_cfg->MemorySpdDataLen = spd_md_len;
	}

	if ((info->topology == SODIMM) || (info->topology == MIXED)) {
		read_sodimm_spd(info, &spd_sodimm_blk);
		if ((info->topology == MIXED) &&
		    (mem_cfg->MemorySpdDataLen != spd_sodimm_blk.len))
			die("Mixed topology has incorrect length.\n");
		else
			mem_cfg->MemorySpdDataLen = spd_sodimm_blk.len;
	}

	for (i = 0; i < DDR4_CHANNELS; i++) {
		ddr4_get_spd(i, &spd_md_data, &spd_sodimm_blk, info,
			     half_populated, &spd_dimm0, &spd_dimm1);
		init_spd_upds(mem_cfg, DDR4_FSP_UPD_CHANNEL_IDX(i), spd_dimm0, spd_dimm1);
	}

	/*
	 * DDR4 memory interface has 8 DQs per channel. Each DQ consists of 8 bits (1
	 * byte). However, FSP UPDs for DQ Map expect a DQ pair (i.e. mapping for 2 bytes) in
	 * each UPD.
	 *
	 * Thus, init_dq_upds() needs to be called for every dq pair of each channel.
	 * DqMapCpu2DramCh0 --> dq_map[CHAN=0][0-1]
	 * DqMapCpu2DramCh1 --> dq_map[CHAN=0][2-3]
	 * DqMapCpu2DramCh2 --> dq_map[CHAN=0][4-5]
	 * DqMapCpu2DramCh3 --> dq_map[CHAN=0][6-7]
	 * DqMapCpu2DramCh4 --> dq_map[CHAN=1][0-1]
	 * DqMapCpu2DramCh5 --> dq_map[CHAN=1][2-3]
	 * DqMapCpu2DramCh6 --> dq_map[CHAN=1][4-5]
	 * DqMapCpu2DramCh7 --> dq_map[CHAN=1][6-7]
	 */

	/*
	 * DDR4 memory interface has 8 DQS pairs per channel. FSP UPDs for DQS Map expect a
	 * pair in each UPD.
	 *
	 * Thus, init_dqs_upds() needs to be called for every dqs pair of each channel.
	 * DqsMapCpu2DramCh0 --> dqs_map[CHAN=0][0-1]
	 * DqsMapCpu2DramCh1 --> dqs_map[CHAN=0][2-3]
	 * DqsMapCpu2DramCh2 --> dqs_map[CHAN=0][4-5]
	 * DqsMapCpu2DramCh3 --> dqs_map[CHAN=0][6-7]
	 * DqsMapCpu2DramCh4 --> dqs_map[CHAN=1][0-1]
	 * DqsMapCpu2DramCh5 --> dqs_map[CHAN=1][2-3]
	 * DqsMapCpu2DramCh6 --> dqs_map[CHAN=1][4-5]
	 * DqsMapCpu2DramCh7 --> dqs_map[CHAN=1][6-7]
	 */

	for (i = 0; i < DDR4_CHANNELS; i++) {
		for (int b = 0; b < DDR4_BYTES_PER_CHANNEL; b += 2) {
			if (half_populated && (i == 1)) {
				init_dq_upds_empty(mem_cfg, index);
				init_dqs_upds_empty(mem_cfg, index);
			} else {
				init_dq_upds(mem_cfg, index, board_cfg->dq_map[i][b],
				     board_cfg->dq_map[i][b+1]);
				init_dqs_upds(mem_cfg, index, board_cfg->dqs_map[i][b],
				      board_cfg->dqs_map[i][b+1]);
			}
			index++;
		}
	}
}

void meminit_ddr(FSP_M_CONFIG *mem_cfg, const struct ddr_memory_cfg *board_cfg,
			const struct spd_info *info, bool half_populated)
{
	switch (board_cfg->mem_type) {
	case MEMTYPE_DDR4:
		meminit_ddr4(mem_cfg, board_cfg->ddr4_cfg, info, half_populated);
		break;
	case MEMTYPE_LPDDR4X:
		meminit_lpddr4x(mem_cfg, board_cfg->lpddr4_cfg, info, half_populated);
		break;
	default:
		die("Unsupported memory type = %d!\n", board_cfg->mem_type);
	}
}
