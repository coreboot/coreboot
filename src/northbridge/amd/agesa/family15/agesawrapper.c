/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
#include <cpu/x86/mtrr.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "cpuRegisters.h"
#include "cpuCacheInit.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "Dispatcher.h"
#include "cpuCacheInit.h"
#include "amdlib.h"
#include "Filecode.h"
#include "heapManager.h"
#include <cpuFamilyTranslation.h>	/* CPU_SPECIFIC_SERVICES */

#define FILECODE UNASSIGNED_FILE_FILECODE

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable = NULL;
VOID *AcpiPstate = NULL;
VOID *AcpiSrat = NULL;
VOID *AcpiSlit = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib = NULL;

/* TODO: Function body should be in mainboard directory. */
void OemCustomizeInitEarly(AMD_EARLY_PARAMS *InitEarly)
{
}

void OemCustomizeInitPost(AMD_POST_PARAMS *InitPost)
{
#if IS_ENABLED(CONFIG_BOARD_AMD_DINAR)
	InitPost->MemConfig.UmaMode = UMA_AUTO;
	InitPost->MemConfig.BottomIo = 0xE0;
	InitPost->MemConfig.UmaSize = 0xE0-0xC0;
#endif
}

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status = AGESA_SUCCESS;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));
	memset(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = NULL;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	AmdResetParams.HtConfig.Depth = 0;

	//MARG34PI disabled AGESA_ENTRY_INIT_RESET by default
	//but we need to call AmdCreateStruct to call HeapManagerInit, or the event log not work
#if (defined AGESA_ENTRY_INIT_RESET) && (AGESA_ENTRY_INIT_RESET == TRUE)
	status = AmdInitReset((AMD_RESET_PARAMS *) AmdParamStruct.NewStructPtr);
#endif
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr;

	/* OEM Should Customize the defaults through this hook */
	OemCustomizeInitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly(AmdEarlyParamsPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **) & FamilySpecificServices,
				    &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_POST_PARAMS *PostParams;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	PostParams = (AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr;

	/* OEM Should Customize the defaults through this hook */
	OemCustomizeInitPost(PostParams);

	status = AmdInitPost(PostParams);
	AGESA_EVENTLOG(status, &PostParams->StdHeader);

	AmdReleaseStruct(&AmdParamStruct);

	/* Initialize heap space */
	EmptyHeap();

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **) & FamilySpecificServices,
				    &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParams;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	EnvParams = (AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitEnv(EnvParams);
	AGESA_EVENTLOG(status, &EnvParams->StdHeader);

	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

VOID *agesawrapper_getlateinitptr(int pick)
{
	switch (pick) {
	case PICK_DMI:
		return DmiTable;

	case PICK_PSTATE:
		return AcpiPstate;

	case PICK_SRAT:
		return AcpiSrat;

	case PICK_SLIT:
		return AcpiSlit;
	case PICK_WHEA_MCE:
		return AcpiWheaMce;
	case PICK_WHEA_CMC:
		return AcpiWheaCmc;
	case PICK_ALIB:
		return AcpiAlib;
	default:
		return NULL;
	}

	return NULL;
}

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	status = AmdInitMid((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk(BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32) AmdLateParamsPtr);

	status = AmdInitLate(AmdLateParamsPtr);
	AGESA_EVENTLOG(status, &AmdLateParamsPtr->StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	DmiTable = AmdLateParamsPtr->DmiTable;
	AcpiPstate = AmdLateParamsPtr->AcpiPState;
	AcpiSrat = AmdLateParamsPtr->AcpiSrat;
	AcpiSlit = AmdLateParamsPtr->AcpiSlit;
	AcpiWheaMce = AmdLateParamsPtr->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParamsPtr->AcpiWheaCmc;
	AcpiAlib = AmdLateParamsPtr->AcpiAlib;

	printk(BIOS_DEBUG, "In %s, AGESA generated ACPI tables:\n"
	       "   DmiTable:%p\n   AcpiPstate: %p\n   AcpiSrat:%p\n   AcpiSlit:%p\n"
	       "   Mce:%p\n   Cmc:%p\n   Alib:%p\n",
	       __func__, DmiTable, AcpiPstate, AcpiSrat, AcpiSlit, AcpiWheaMce, AcpiWheaCmc, AcpiAlib);

	/* Don't release the structure until coreboot has copied the ACPI tables.
	 * AmdReleaseStruct (&AmdLateParams);
	 */

	return status;
}

/**
 * @param[in] UINTN ApicIdOfCore,
 * @param[in] AP_EXE_PARAMS *LaunchApParams
 */
AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, VOID * ConfigPtr)
{
	AGESA_STATUS status;
	AMD_LATE_PARAMS AmdLateParams;

	memset(&AmdLateParams, 0, sizeof(AMD_LATE_PARAMS));

	AmdLateParams.StdHeader.AltImageBasePtr = 0;
	AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdLateParams.StdHeader.Func = 0;
	AmdLateParams.StdHeader.ImageBasePtr = 0;
	AmdLateParams.StdHeader.HeapStatus = HEAP_TEMP_MEM;

	printk(BIOS_DEBUG, "AmdLateRunApTask on Core: %x\n", (uint32_t) Data);
	status = AmdLateRunApTask((AP_EXE_PARAMS *) ConfigPtr);
	AGESA_EVENTLOG(status, &AmdLateParams.StdHeader);
	ASSERT((status == AGESA_SUCCESS) || (status == AGESA_UNSUPPORTED));

	return status;
}
