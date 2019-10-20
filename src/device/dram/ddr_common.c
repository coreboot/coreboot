/*
 * This file is part of the coreboot project.
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

#include <device/dram/common.h>
#include <types.h>

/**
 * \brief Calculate the CRC of a DDR SPD data
 *
 * @param spd pointer to raw SPD data
 * @param len length of data in SPD
 *
 * @return the CRC of the SPD data
 */
u16 ddr_crc16(const u8 *ptr, int n_crc)
{
	int i;
	u16 crc = 0;

	while (--n_crc >= 0) {
		crc = crc ^ ((int)*ptr++ << 8);
		for (i = 0; i < 8; ++i)
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;
	}

	return crc;
}
