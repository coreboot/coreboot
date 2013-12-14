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

#include "agesawrapper.h"
#include <lib.h>

static void calc_fake_spd_crc(unsigned char *SPDPtr, unsigned short *crc);

int Read_SPD_from_file(void *infoptr)
{
	unsigned char *spd_ptr;
	unsigned short index, crc;
	#include CONFIG_PATH_TO_SPD_FILE
	AGESA_READ_SPD_PARAMS *info = infoptr;

	if (info->MemChannelId > CONFIG_DDR3_CHANNEL_MAX)
		return 1;
	if (info->SocketId != 0)
		return 1;
	if (info->DimmId != 0)
		return 1;

	/* read the bytes from the table */
	spd_ptr = (unsigned char *)info->Buffer;
	for (index = 0; index < 128; index++)
		spd_ptr[index] = ddr3_fake_spd[index];

	/* If CRC bytes are zeroes, calculate and store the CRC of the fake table */
	if ((spd_ptr[126] == 0) && (spd_ptr[127] == 0)) {
		calc_fake_spd_crc(spd_ptr, &crc);
		spd_ptr[126] = (unsigned char)(crc & 0xFF);
		spd_ptr[127] = (unsigned char)(crc >> 8);
	}
	return 0;
}

static void calc_fake_spd_crc(unsigned char *SPDPtr, unsigned short *crc)
{
	int i, j, jmax;

	/* should the CRC be done on bytes 0-116 or 0-125 ? */
	if (SPDPtr[0] & 0x80)
		 jmax = 117;
	else
		jmax = 126;

	*crc = 0;
	for (j = 0; j < jmax; j++) {
		*crc = *crc ^ ((unsigned short)SPDPtr[j] << 8);
		for (i = 0; i < 8; i++) {
			if (*crc & 0x8000)
				*crc = (*crc << 1) ^ 0x1021;
			else
				*crc = (*crc << 1);
		}
	}
}
