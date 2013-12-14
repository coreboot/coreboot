/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <lib.h>

int read_spd_from_file(unsigned char *spd_ptr)
{
	unsigned short index, crc = 0;
	int i, j, jmax;
	#include CONFIG_PATH_TO_SPD_FILE

	/* read the bytes from the table */
	for (index = 0; index < 128; index++)
		spd_ptr[index] = ddr3_fake_spd[index];

	/* If CRC bytes are zeroes, calculate and store the CRC of the fake table */
	if ((spd_ptr[126] == 0) && (spd_ptr[127] == 0)) {
		/* should the CRC be done on bytes 0-116 or 0-125 ? */
		if (spd_ptr[0] & 0x80)
			jmax = 117;
		else
			jmax = 126;

		for (j = 0; j < jmax; j++) {
			crc = crc ^ ((unsigned short)spd_ptr[j] << 8);
			for (i = 0; i < 8; i++) {
				if (crc & 0x8000)
					crc = (crc << 1) ^ 0x1021;
				else
					crc = (crc << 1);
			}
		}
		spd_ptr[126] = (unsigned char)(crc & 0xff);
		spd_ptr[127] = (unsigned char)(crc >> 8);
	}
	return 0;
}
