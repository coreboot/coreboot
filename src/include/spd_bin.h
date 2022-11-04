/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SPD_BIN_H
#define SPD_BIN_H

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
#define  SPD_DRAM_LPDDR4	0x10
#define  SPD_DRAM_LPDDR4X	0x11
#define  SPD_DRAM_DDR5		0x12
#define  SPD_DRAM_LPDDR5	0x13
#define  SPD_DRAM_LPDDR5X	0x15
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_SN_LEN		4
#define DDR3_ORGANIZATION	7
#define DDR3_BUS_DEV_WIDTH	8
#define DDR4_ORGANIZATION	12
#define DDR4_BUS_DEV_WIDTH	13
#define DDR3_SPD_PART_OFF	128
#define DDR3_SPD_PART_LEN	18
#define DDR3_SPD_SN_OFF		122
#define LPDDR3_SPD_PART_OFF	128
#define LPDDR3_SPD_PART_LEN	18
#define DDR4_SPD_PART_OFF	329
#define DDR4_SPD_PART_LEN	20
#define DDR4_SPD_SN_OFF		325

struct spd_block {
	u8 addr_map[CONFIG_DIMM_MAX]; /* 7 bit I2C addresses */
	u8 *spd_array[CONFIG_DIMM_MAX];
	/* Length of each dimm */
	u16 len;
};

void print_spd_info(uint8_t spd[]);
uintptr_t spd_cbfs_map(u8 spd_index);
void dump_spd_info(struct spd_block *blk);
void get_spd_smbus(struct spd_block *blk);

/*
 * get_spd_sn returns the SODIMM serial number. It only supports DDR3 and DDR4.
 *  return CB_SUCCESS, sn is the serial number and sn=0xffffffff if the dimm is not present.
 *  return CB_ERR, if dram_type is not supported or addr is a zero.
 */
enum cb_err get_spd_sn(u8 addr, u32 *sn);

/* expects SPD size to be 128 bytes, reads from "spd.bin" in CBFS and
   verifies the checksum. Only available if CONFIG_DIMM_SPD_SIZE == 128. */
int read_ddr3_spd_from_cbfs(u8 *buf, int idx);
#endif
