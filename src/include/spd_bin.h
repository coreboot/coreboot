/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SPD_BIN_H
#define SPD_BIN_H

#include <stdint.h>
#include <commonlib/region.h>

#define SPD_PAGE_0		(0x6C >> 1)
#define SPD_PAGE_1		(0x6E >> 1)
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_SN_LEN		4
#define DDR3_ORGANIZATION	7
#define DDR3_BUS_DEV_WIDTH	8
#define DDR4_ORGANIZATION	12
#define DDR4_BUS_DEV_WIDTH	13
#define DDR4_SPD_SN_OFF		325
#define MAX_SPD_PAGE_SIZE_SPD5	128
#define MAX_SPD_SIZE		(256 * 4)
#define SPD_HUB_MEMREG(addr)	((u8)(0x80 | (addr)))
#define SPD5_MR11		0x0B
#define SPD5_MR0		0x00
#define SPD5_MEMREG_REG(addr)	((u8)((~0x80) & (addr)))
#define SPD5_MR0_SPD5_HUB_DEV	0x51

struct spd_offset_table {
	u16 start;		/* Offset 0 */
	u16 end;		/* Offset 2 */
};

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

int spd_read_byte(u8 slave_addr, u8 bus_addr);
int spd_read_word(u8 slave_addr, u8 bus_addr);
void spd_write_byte(u8 slave_addr, u8 bus_addr, u8 value);

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
