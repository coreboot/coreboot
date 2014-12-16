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
#include "FchPlatform.h"
#include "Fch.h"
#include <cpu/amd/agesa/s3_resume.h>
#include <arch/io.h>
#include <device/device.h>
#include "hudson.h"

#define FILECODE UNASSIGNED_FILE_FILECODE

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
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
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
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly((AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr);
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
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	PostParams = (AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitPost(PostParams);
	AGESA_EVENTLOG(status, &PostParams->StdHeader);
	AmdReleaseStruct(&AmdParamStruct);
	/* Initialize heap space */
	EmptyHeap();

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParam;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr;

	status = AmdInitEnv(EnvParam);
	AGESA_EVENTLOG(status, &EnvParam->StdHeader);

	return status;
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

	AmdCreateStruct(&AmdParamStruct);

	((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr)->GnbMidConfiguration.iGpuVgaMode = 0;	/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	status = AmdInitMid((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}
