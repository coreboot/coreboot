/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "heapManager.h"
#include "SB800.h"
#include <stdlib.h>
#include <cbfs.h>
#include <string.h>
#include <device/dram/ddr3.h>

#define SPD_SIZE    128
#define SPD_CRC_HI  127
#define SPD_CRC_LO  126

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,			agesa_AllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,		agesa_DeallocateBuffer },
	{AGESA_LOCATE_BUFFER,			agesa_LocateBuffer },
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			BiosReadSpd_from_cbfs },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

AGESA_STATUS BiosReadSpd_from_cbfs(UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
#ifdef __PRE_RAM__
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;
	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	char *spd_file;
	size_t spd_file_len;

	printk(BIOS_DEBUG, "read SPD\n");
	spd_file = cbfs_get_file_content(CBFS_DEFAULT_MEDIA, "spd.bin", 0xab,
					 &spd_file_len);
	if (!spd_file)
		die("file [spd.bin] not found in CBFS");
	if (spd_file_len < SPD_SIZE)
		die("Missing SPD data.");

	memcpy((char*)info->Buffer, spd_file, SPD_SIZE);

	u16 crc = spd_ddr3_calc_crc(info->Buffer, SPD_SIZE);

	if (((info->Buffer[SPD_CRC_LO] == 0) && (info->Buffer[SPD_CRC_HI] == 0))
	 || (info->Buffer[SPD_CRC_LO] != (crc & 0xff))
	 || (info->Buffer[SPD_CRC_HI] != (crc >> 8))) {
		printk(BIOS_WARNING, "SPD has a invalid or zero-valued CRC\n");
		info->Buffer[SPD_CRC_LO] = crc & 0xff;
		info->Buffer[SPD_CRC_HI] = crc >> 8;
		u16 i;
		printk(BIOS_WARNING, "\nDisplay the SPD");
		for (i = 0; i < SPD_SIZE; i++) {
			if((i % 16) == 0x00)
				printk(BIOS_WARNING, "\n%02x:  ",i);
			printk(BIOS_WARNING, "%02x ", info->Buffer[i]);
		}
		printk(BIOS_WARNING, "\n");
	 }
	Status = AGESA_SUCCESS;
#else
	Status = AGESA_UNSUPPORTED;
#endif

	return Status;
}

/*	Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	// Unlike AMD/Inagua, this board is unable to vary the RAM voltage.
	// Make sure the right speed settings are selected.
	((MEM_DATA_STRUCT*)ConfigPtr)->ParameterListPtr->DDR3Voltage = VOLT1_5;
	return AGESA_SUCCESS;
}
