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

#include <arch/byteorder.h>
#include <cbfs.h>
#include <console/console.h>
#include <spd_bin.h>
#include <string.h>
#include <device/early_smbus.h>
#include <device/dram/ddr3.h>

static u8 spd_data[CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE] CAR_GLOBAL;

void dump_spd_info(struct spd_block *blk)
{
	u8 i;

	for (i = 0; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL && blk->spd_array[i][0] != 0) {
			printk(BIOS_DEBUG, "SPD @ 0x%02X\n", 0xA0|(i << 1));
			print_spd_info(blk->spd_array[i]);
		}
}

void print_spd_info(uint8_t spd[])
{
	const int spd_banks[8] = { 8, 16, 32, 64, -1, -1, -1, -1 };
	const int spd_capmb[8] = { 1,  2,  4,  8, 16, 32, 64, 128 };
	const int spd_rows[8]  = {12, 13, 14, 15, 16, 17, -1, -1 };
	const int spd_cols[8]  = { 9, 10, 11, 12, -1, -1, -1, -1 };
	const int spd_ranks[8] = { 1,  2,  3,  4, -1, -1, -1, -1 };
	const int spd_devw[8]  = { 4,  8, 16, 32, -1, -1, -1, -1 };
	const int spd_busw[8]  = { 8, 16, 32, 64, -1, -1, -1, -1 };
	char spd_name[DDR4_SPD_PART_LEN+1] = { 0 };

	int banks = spd_banks[(spd[SPD_DENSITY_BANKS] >> 4) & 7];
	int capmb = spd_capmb[spd[SPD_DENSITY_BANKS] & 7] * 256;
	int rows  = spd_rows[(spd[SPD_ADDRESSING] >> 3) & 7];
	int cols  = spd_cols[spd[SPD_ADDRESSING] & 7];
	int ranks = spd_ranks[(spd[SPD_ORGANIZATION] >> 3) & 7];
	int devw  = spd_devw[spd[SPD_ORGANIZATION] & 7];
	int busw  = spd_busw[spd[SPD_BUS_DEV_WIDTH] & 7];

	/* Module type */
	printk(BIOS_INFO, "SPD: module type is ");
	switch (spd[SPD_DRAM_TYPE]) {
	case SPD_DRAM_DDR3:
		printk(BIOS_INFO, "DDR3\n");
		/* Module Part Number */
		memcpy(spd_name, &spd[DDR3_SPD_PART_OFF], DDR3_SPD_PART_LEN);
		spd_name[DDR3_SPD_PART_LEN] = 0;
		break;
	case SPD_DRAM_LPDDR3_INTEL:
	case SPD_DRAM_LPDDR3_JEDEC:
		printk(BIOS_INFO, "LPDDR3\n");
		/* Module Part Number */
		memcpy(spd_name, &spd[LPDDR3_SPD_PART_OFF],
			LPDDR3_SPD_PART_LEN);
		spd_name[LPDDR3_SPD_PART_LEN] = 0;
		break;
	case SPD_DRAM_DDR4:
		printk(BIOS_INFO, "DDR4\n");
		memcpy(spd_name, &spd[DDR4_SPD_PART_OFF], DDR4_SPD_PART_LEN);
		spd_name[DDR4_SPD_PART_LEN] = 0;
		ranks = (spd[SPD_ORGANIZATION] >> 3) & 7;
		devw  = spd_devw[spd[12] & 7];
		busw  = spd_busw[spd[13] & 7];
		break;
	default:
		printk(BIOS_INFO, "Unknown (%02x)\n", spd[SPD_DRAM_TYPE]);
		break;
	}

	printk(BIOS_INFO, "SPD: module part is %s\n", spd_name);

	printk(BIOS_INFO,
		"SPD: banks %d, ranks %d, rows %d, columns %d, density %d Mb\n",
		banks, ranks, rows, cols, capmb);
	printk(BIOS_INFO, "SPD: device width %d bits, bus width %d bits\n",
		devw, busw);

	if (capmb > 0 && busw > 0 && devw > 0 && ranks > 0) {
		/* SIZE = DENSITY / 8 * BUS_WIDTH / SDRAM_WIDTH * RANKS */
		printk(BIOS_INFO, "SPD: module size is %u MB (per channel)\n",
			capmb / 8 * busw / devw * ranks);
	}
}

static void update_spd_len(struct spd_block *blk)
{
	u8 i, j = 0;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL)
			j |= blk->spd_array[i][SPD_DRAM_TYPE];

	/* If spd used is DDR4, then its length is 512 byte. */
	if (j == SPD_DRAM_DDR4)
		blk->len = SPD_PAGE_LEN_DDR4;
	else
		blk->len = SPD_PAGE_LEN;
}

int get_spd_cbfs_rdev(struct region_device *spd_rdev, u8 spd_index)
{
	struct cbfsf fh;

	uint32_t cbfs_type = CBFS_TYPE_SPD;

	if (cbfs_boot_locate(&fh, "spd.bin", &cbfs_type) < 0)
		return -1;
	cbfs_file_data(spd_rdev, &fh);
	return rdev_chain(spd_rdev, spd_rdev, spd_index * CONFIG_DIMM_SPD_SIZE,
							CONFIG_DIMM_SPD_SIZE);
}

static void get_spd(u8 *spd, u8 addr)
{
	u16 i;
	/* Assuming addr is 8 bit address, make it 7 bit */
	addr = addr >> 1;
	if (smbus_read_byte(0, addr, 0)  == 0xff) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n",
			addr << 1);
		/* Make sure spd is zeroed if dimm doesn't exist. */
		memset(spd, 0, CONFIG_DIMM_SPD_SIZE);
		return;
	}

	for (i = 0; i < SPD_PAGE_LEN; i++)
		spd[i] = smbus_read_byte(0, addr, i);
	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (spd[SPD_DRAM_TYPE] == SPD_DRAM_DDR4 &&
		CONFIG_DIMM_SPD_SIZE >= SPD_DRAM_DDR4) {
		/* Switch to page 1 */
		smbus_write_byte(0, SPD_PAGE_1, 0, 0);
		for (i = 0; i < SPD_PAGE_LEN; i++)
			spd[i+SPD_PAGE_LEN] = smbus_read_byte(0, addr, i);
		/* Restore to page 0 */
		smbus_write_byte(0, SPD_PAGE_0, 0, 0);
	}
}

void get_spd_smbus(struct spd_block *blk)
{
	u8 i;
	unsigned char *spd_data_ptr = car_get_var_ptr(&spd_data);

	for (i = 0 ; i < CONFIG_DIMM_MAX; i++) {
		get_spd(spd_data_ptr + i * CONFIG_DIMM_SPD_SIZE,
			0xA0 + (i << 1));
		blk->spd_array[i] = spd_data_ptr + i * CONFIG_DIMM_SPD_SIZE;
	}

	update_spd_len(blk);
}

#if CONFIG_DIMM_SPD_SIZE == 128
int read_ddr3_spd_from_cbfs(u8 *buf, int idx)
{
	const int SPD_CRC_HI = 127;
	const int SPD_CRC_LO = 126;

	const char *spd_file;
	size_t spd_file_len = 0;
	size_t min_len = (idx + 1) * CONFIG_DIMM_SPD_SIZE;

	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_file_len);
	if (!spd_file)
		printk(BIOS_EMERG, "file [spd.bin] not found in CBFS");
	if (spd_file_len < min_len)
		printk(BIOS_EMERG, "Missing SPD data.");
	if (!spd_file || spd_file_len < min_len)
		return -1;

	memcpy(buf, spd_file + (idx * CONFIG_DIMM_SPD_SIZE), CONFIG_DIMM_SPD_SIZE);

	u16 crc = spd_ddr3_calc_crc(buf, CONFIG_DIMM_SPD_SIZE);

	if (((buf[SPD_CRC_LO] == 0) && (buf[SPD_CRC_HI] == 0))
	 || (buf[SPD_CRC_LO] != (crc & 0xff))
	 || (buf[SPD_CRC_HI] != (crc >> 8))) {
		printk(BIOS_WARNING, "SPD CRC %02x%02x is invalid, should be %04x\n",
			buf[SPD_CRC_HI], buf[SPD_CRC_LO], crc);
		buf[SPD_CRC_LO] = crc & 0xff;
		buf[SPD_CRC_HI] = crc >> 8;
		u16 i;
		printk(BIOS_WARNING, "\nDisplay the SPD");
		for (i = 0; i < CONFIG_DIMM_SPD_SIZE; i++) {
			if ((i % 16) == 0x00)
				printk(BIOS_WARNING, "\n%02x:  ", i);
			printk(BIOS_WARNING, "%02x ", buf[i]);
		}
		printk(BIOS_WARNING, "\n");
	 }
	 return 0;
}
#endif
