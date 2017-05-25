/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SPD_BIN_H
#define SPD_BIN_H

#include <arch/early_variables.h>
#include <stdint.h>
#include <commonlib/region.h>

#define SPD_PAGE_LEN		256
#define SPD_PAGE_LEN_DDR4	512
#define SPD_PAGE_0		(0x6C >> 1)
#define SPD_PAGE_1		(0x6E >> 1)
#define SPD_DRAM_TYPE		2
#define  SPD_DRAM_DDR3		0x0B
#define  SPD_DRAM_LPDDR3_INTEL	0xF1
#define  SPD_DRAM_LPDDR3_JEDEC	0x0F
#define  SPD_DRAM_DDR4		0x0C
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define DDR3_SPD_PART_OFF	128
#define DDR3_SPD_PART_LEN	18
#define LPDDR3_SPD_PART_OFF	128
#define LPDDR3_SPD_PART_LEN	18
#define DDR4_SPD_PART_OFF	329
#define DDR4_SPD_PART_LEN	20
#define LPDDR4_SPD_PART_OFF	329
#define LPDDR4_SPD_PART_LEN	20

struct spd_block {
	u8 addr_map[CONFIG_DIMM_MAX];
	u8 *spd_array[CONFIG_DIMM_MAX];
	/* Length of each dimm */
	u16 len;
};

void print_spd_info(uint8_t spd[]);
/* Return 0 on success & -1 on failure */
int get_spd_cbfs_rdev(struct region_device *spd_rdev, u8 spd_index);
void dump_spd_info(struct spd_block *blk);
void get_spd_smbus(struct spd_block *blk);

/* expects SPD size to be 128 bytes, reads from "spd.bin" in CBFS and
   verifies the checksum. Only available if CONFIG_DIMM_SPD_SIZE == 128. */
int read_ddr3_spd_from_cbfs(u8 *buf, int idx);
#endif
