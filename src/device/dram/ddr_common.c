/* SPDX-License-Identifier: GPL-2.0-only */

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
