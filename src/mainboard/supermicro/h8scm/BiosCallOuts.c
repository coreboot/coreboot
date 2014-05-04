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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include <northbridge/amd/agesa/family15/dimmSpd.h>

STATIC BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,			BiosAllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,		BiosDeallocateBuffer },
	{AGESA_DO_RESET,			BiosReset },
	{AGESA_LOCATE_BUFFER,			BiosLocateBuffer },
	{AGESA_READ_SPD,			BiosReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			BiosRunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,		agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN i;
	AGESA_STATUS CalloutStatus;
	UINTN CallOutCount = sizeof (BiosCallouts) / sizeof (BiosCallouts [0]);

	for (i = 0; i < CallOutCount; i++) {
		if (BiosCallouts[i].CalloutName == Func) {
			break;
		}
	}

	if(i >= CallOutCount) {
		return AGESA_UNSUPPORTED;
	}

	CalloutStatus = BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);

	return CalloutStatus;
}



AGESA_STATUS BiosReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
#ifdef __PRE_RAM__
	Status = agesa_ReadSPD (Func, Data, ConfigPtr);
#else
	Status = AGESA_UNSUPPORTED;
#endif

	return Status;
}
