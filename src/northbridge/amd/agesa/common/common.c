/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

#include "AGESA.h"
#include "common.h"
#include <device/dram/ddr3.h>
#include <string.h>
#include <cbfs.h>

AGESA_STATUS common_ReadCbfsSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
#ifdef __PRE_RAM__
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;
	size_t spd_file_length;

	if (info->MemChannelId > CONFIG_AGESA_DDR3_CHANNEL_MAX)
		return AGESA_ERROR;
	if (info->SocketId != 0)
		return AGESA_ERROR;
	if (info->DimmId != 0)
		return AGESA_ERROR;

	char *spd_file;

	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD_BIN,
						&spd_file_length);
	if (!spd_file)
		die("file [spd.bin] not found in CBFS");

	printk(BIOS_DEBUG, "\nCBFS SPD file length = 0x%x bytes\n", (unsigned int)spd_file_length);

	if (CONFIG_MULTIPLE_DDR_SPD) {
		struct multi_spd_info *spd_info = (struct multi_spd_info *)info->Buffer;
		printk(BIOS_DEBUG, "Multiple DDR SPD: using offset %d\n", spd_info->offset);
		if (spd_info->offset > (spd_file_length / spd_info->size))
			printk(BIOS_EMERG, "Multiple SPD offset is greater than SPD length\n");
		else {
			spd_file += spd_info->offset * spd_info->size;
			spd_file_length = spd_info->size;
		}
	}
	memcpy((char*)info->Buffer, spd_file, spd_file_length);

	u16 crc = spd_ddr3_calc_crc(info->Buffer, spd_file_length);
	if (crc == 0){
		printk(BIOS_EMERG, "Error: Unable to calculate CRC on SPD\n");
		return AGESA_UNSUPPORTED;
	}
	if (((info->Buffer[SPD_CRC_LO] == 0) && (info->Buffer[SPD_CRC_HI] == 0))
	  || (info->Buffer[SPD_CRC_LO] != (crc & 0xff))
	  || (info->Buffer[SPD_CRC_HI] != (crc >> 8))) {
		printk(BIOS_WARNING, "SPD CRC %02x%02x is invalid, should be %04x\n",
			info->Buffer[SPD_CRC_HI], info->Buffer[SPD_CRC_LO], crc);
		info->Buffer[SPD_CRC_LO] = crc & 0xff;
		info->Buffer[SPD_CRC_HI] = crc >> 8;
		u16 i;
		printk(BIOS_SPEW, "\nDisplay the SPD");
		for (i = 0; i < spd_file_length; i++) {
			if((i % 16) == 0x00)
				printk(BIOS_SPEW, "\n%02x:  ",i);
			printk(BIOS_SPEW, "%02x ", info->Buffer[i]);
		}
		printk(BIOS_SPEW, "\n");
	}
	return AGESA_SUCCESS;
#else
	return AGESA_UNSUPPORTED;
#endif
}
