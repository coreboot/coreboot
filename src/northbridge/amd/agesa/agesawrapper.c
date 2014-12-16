/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <string.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "amdlib.h"

#include "heapManager.h"

#if !defined(__PRE_RAM__)

static AMD_LATE_PARAMS *AmdLateParams = NULL;

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY15TN) || IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY15RL) || \
	IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY16KB)
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
#endif

	AmdCreateStruct(&AmdParamStruct);
	AmdLateParams = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitLate(AmdLateParams);
	AGESA_EVENTLOG(status, &AmdLateParams->StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	/* We will reference AmdLateParams when coreboot copies the ACPI tables,
	 * so must not call AmdReleaseStruct(&AmdParamStruct) here.
	 */
	return status;
}

void *agesawrapper_getlateinitptr(int pick)
{
	ASSERT(AmdLateParams != NULL);

	switch (pick) {
	case PICK_DMI:
		return AmdLateParams->DmiTable;
	case PICK_PSTATE:
		return AmdLateParams->AcpiPState;
	case PICK_SRAT:
		return AmdLateParams->AcpiSrat;
	case PICK_SLIT:
		return AmdLateParams->AcpiSlit;
	case PICK_WHEA_MCE:
		return AmdLateParams->AcpiWheaMce;
	case PICK_WHEA_CMC:
		return AmdLateParams->AcpiWheaCmc;
	case PICK_ALIB:
		return AmdLateParams->AcpiAlib;
	case PICK_IVRS:
#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY14)
		return NULL;
#else
		return AmdLateParams->AcpiIvrs;
#endif
	default:
		return NULL;
	}
	return NULL;
}

#endif /* __PRE_RAM__ */

AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, VOID * ConfigPtr)
{
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(AP_EXE_PARAMS));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = AmdLateRunApTask(&ApExeParams);
	AGESA_EVENTLOG(status, &ApExeParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}
