/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <cbfs.h>
#include <console/console.h>
#include <device/dram/ddr3.h>
#include <spd_cache.h>
#include <stdint.h>
#include <string.h>

#define SPD_SIZE	128
#define SPD_CRC_HI	127
#define SPD_CRC_LO	126

int read_spd_from_cbfs(u8 *buf, int idx)
{
	const char *spd_file;
	size_t spd_file_len = 0;
	size_t min_len = (idx + 1) * SPD_SIZE;

	printk(BIOS_DEBUG, "read SPD\n");
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_file_len);
	if (!spd_file)
		printk(BIOS_EMERG, "file [spd.bin] not found in CBFS");
	if (spd_file_len < min_len)
		printk(BIOS_EMERG, "Missing SPD data.");
	if (!spd_file || spd_file_len < min_len)
		return -1;

	memcpy(buf, spd_file + (idx * SPD_SIZE), SPD_SIZE);

	u16 crc = spd_ddr3_calc_crc(buf, SPD_SIZE);

	if (((buf[SPD_CRC_LO] == 0) && (buf[SPD_CRC_HI] == 0))
	 || (buf[SPD_CRC_LO] != (crc & 0xff))
	 || (buf[SPD_CRC_HI] != (crc >> 8))) {
		printk(BIOS_WARNING, "SPD CRC %02x%02x is invalid, should be %04x\n",
			buf[SPD_CRC_HI], buf[SPD_CRC_LO], crc);
		buf[SPD_CRC_LO] = crc & 0xff;
		buf[SPD_CRC_HI] = crc >> 8;
		u16 i;
		printk(BIOS_WARNING, "\nDisplay the SPD");
		for (i = 0; i < SPD_SIZE; i++) {
			if((i % 16) == 0x00)
				printk(BIOS_WARNING, "\n%02x:  ", i);
			printk(BIOS_WARNING, "%02x ", buf[i]);
		}
		printk(BIOS_WARNING, "\n");
	 }
	 return 0;
}
