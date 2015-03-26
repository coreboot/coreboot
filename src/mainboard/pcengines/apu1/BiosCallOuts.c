/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include "AGESA.h"
#include "amdlib.h"
#include <spd_cache.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "heapManager.h"
#include "SB800.h"
#include <stdlib.h>
#include "gpio_ftns.h"

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINT32 Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			board_ReadSpd_from_cbfs },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/*	Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	// Unlike e.g. AMD Inagua, Persimmon is unable to vary the RAM voltage.
	// Make sure the right speed settings are selected.
	((MEM_DATA_STRUCT*)ConfigPtr)->ParameterListPtr->DDR3Voltage = VOLT1_5;
	return AGESA_SUCCESS;
}

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;
#ifdef __PRE_RAM__
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;
	u8 index = get_spd_offset();

	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_spd_from_cbfs((u8*)info->Buffer, index) < 0)
		die("No SPD data\n");

	Status = AGESA_SUCCESS;
#endif
	return Status;
}
