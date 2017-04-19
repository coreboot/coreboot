/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 Advanced Micro Devices, Inc.
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

#include <cbmem.h>
#include <stdint.h>
#include <string.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "amdlib.h"
#include <cpu/amd/agesa/s3_resume.h>

#include "heapManager.h"

static const struct OEM_HOOK *OemHook = &OemCustomize;

#if defined(__PRE_RAM__)

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));
	memset(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;

	status = AmdInitReset((AMD_RESET_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	/* OEM Should Customize the defaults through this hook. */
	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr;
	if (OemHook->InitEarly)
		OemHook->InitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly(AmdEarlyParamsPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);

	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_POST_PARAMS *PostParams;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);

	/* OEM Should Customize the defaults through this hook. */
	PostParams = (AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr;
	if (OemHook->InitPost)
		OemHook->InitPost(PostParams);

	status = AmdInitPost(PostParams);
	AGESA_EVENTLOG(status, &PostParams->StdHeader);

	backup_top_of_low_cacheable(PostParams->MemConfig.Sub4GCacheTop);

	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitresume(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESUME_PARAMS *AmdResumeParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESUME;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	AmdResumeParamsPtr = (AMD_RESUME_PARAMS *) AmdParamStruct.NewStructPtr;

	AmdResumeParamsPtr->S3DataBlock.NvStorageSize = 0;
	AmdResumeParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	OemInitResume(&AmdResumeParamsPtr->S3DataBlock);

	status = AmdInitResume(AmdResumeParamsPtr);

	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParam;

	/* Initialize heap space */
	EmptyHeap();

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr;

	status = AmdInitEnv(EnvParam);
	AGESA_EVENTLOG(status, &EnvParam->StdHeader);

	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amds3laterestore(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdInterfaceParams;
	AMD_S3LATE_PARAMS AmdS3LateParams;
	AMD_S3LATE_PARAMS *AmdS3LateParamsPtr;

	memset(&AmdS3LateParams, 0, sizeof(AMD_S3LATE_PARAMS));

	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.AllocationMethod = ByHost;
	AmdInterfaceParams.AgesaFunctionName = AMD_S3LATE_RESTORE;
	AmdInterfaceParams.NewStructPtr = &AmdS3LateParams;
	AmdInterfaceParams.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdS3LateParamsPtr = &AmdS3LateParams;
	AmdInterfaceParams.NewStructSize = sizeof(AMD_S3LATE_PARAMS);

	AmdCreateStruct(&AmdInterfaceParams);

#if 0
	/* TODO: What to do with NvStorage here? */
	AmdS3LateParamsPtr->S3DataBlock.NvStorageSize = 0;
#endif
	AmdS3LateParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	OemS3LateRestore(&AmdS3LateParamsPtr->S3DataBlock);

	status = AmdS3LateRestore(AmdS3LateParamsPtr);
	AGESA_EVENTLOG(status, &AmdInterfaceParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}

#else /* __PRE_RAM__ */

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_MID_PARAMS *MidParam;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);

	/* OEM Should Customize the defaults through this hook. */
	MidParam = (AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr;
	if (OemHook->InitMid)
		OemHook->InitMid(MidParam);

	status = AmdInitMid(MidParam);
	AGESA_EVENTLOG(status, &MidParam->StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdS3Save(void)
{
	AGESA_STATUS status;
	AMD_S3SAVE_PARAMS *AmdS3SaveParamsPtr;
	AMD_INTERFACE_PARAMS AmdInterfaceParams;

	memset(&AmdInterfaceParams, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdInterfaceParams.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdInterfaceParams.AllocationMethod = PostMemDram;
	AmdInterfaceParams.AgesaFunctionName = AMD_S3_SAVE;
	AmdInterfaceParams.StdHeader.AltImageBasePtr = 0;
	AmdInterfaceParams.StdHeader.Func = 0;
	AmdCreateStruct(&AmdInterfaceParams);

	AmdS3SaveParamsPtr = (AMD_S3SAVE_PARAMS *) AmdInterfaceParams.NewStructPtr;
	AmdS3SaveParamsPtr->StdHeader = AmdInterfaceParams.StdHeader;

	status = AmdS3Save(AmdS3SaveParamsPtr);
	AGESA_EVENTLOG(status, &AmdInterfaceParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	OemS3Save(&AmdS3SaveParamsPtr->S3DataBlock);

	AmdReleaseStruct(&AmdInterfaceParams);

	return status;
}

/* We will reference AmdLateParams later to copy ACPI tables. */
static AMD_LATE_PARAMS *AmdLateParams = NULL;

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY15_TN) || IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY15_RL) || \
	IS_ENABLED(CONFIG_CPU_AMD_AGESA_FAMILY16_KB)
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
#endif

	AmdCreateStruct(&AmdParamStruct);
	AmdLateParams = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitLate(AmdLateParams);
	AGESA_EVENTLOG(status, &AmdLateParams->StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	/* No AmdReleaseStruct(&AmdParamStruct), we need AmdLateParams later. */
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
	ApExeParams.StdHeader.CalloutPtr = &GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = AmdLateRunApTask(&ApExeParams);
	AGESA_EVENTLOG(status, &ApExeParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}
