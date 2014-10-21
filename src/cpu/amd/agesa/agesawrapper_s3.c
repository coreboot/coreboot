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

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <stdint.h>
#include <string.h>

#include <cpu/amd/agesa/s3_resume.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "heapManager.h"

AGESA_STATUS agesawrapper_amdinitresume(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESUME_PARAMS *AmdResumeParamsPtr;
	S3_DATA_TYPE S3DataType;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESUME;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	AmdResumeParamsPtr = (AMD_RESUME_PARAMS *) AmdParamStruct.NewStructPtr;

	AmdResumeParamsPtr->S3DataBlock.NvStorageSize = 0;
	AmdResumeParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	S3DataType = S3DataTypeNonVolatile;

	OemAgesaGetS3Info(S3DataType,
			  (u32 *) & AmdResumeParamsPtr->S3DataBlock.NvStorageSize,
			  (void **)&AmdResumeParamsPtr->S3DataBlock.NvStorage);

	status = AmdInitResume((AMD_RESUME_PARAMS *) AmdParamStruct.NewStructPtr);

	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amds3laterestore(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdInterfaceParams;
	AMD_S3LATE_PARAMS AmdS3LateParams;
	AMD_S3LATE_PARAMS *AmdS3LateParamsPtr;
	S3_DATA_TYPE S3DataType;

	memset(&AmdS3LateParams, 0, sizeof(AMD_S3LATE_PARAMS));

	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.AllocationMethod = ByHost;
	AmdInterfaceParams.AgesaFunctionName = AMD_S3LATE_RESTORE;
	AmdInterfaceParams.NewStructPtr = &AmdS3LateParams;
	AmdInterfaceParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdS3LateParamsPtr = &AmdS3LateParams;
	AmdInterfaceParams.NewStructSize = sizeof(AMD_S3LATE_PARAMS);

	AmdCreateStruct(&AmdInterfaceParams);

	AmdS3LateParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	S3DataType = S3DataTypeVolatile;

	OemAgesaGetS3Info(S3DataType,
			  (u32 *) & AmdS3LateParamsPtr->S3DataBlock.VolatileStorageSize,
			  (void **)&AmdS3LateParamsPtr->S3DataBlock.VolatileStorage);

	status = AmdS3LateRestore(AmdS3LateParamsPtr);
	AGESA_EVENTLOG(status, &AmdInterfaceParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}

#ifndef __PRE_RAM__
AGESA_STATUS agesawrapper_amdS3Save(void)
{
	AGESA_STATUS status;
	AMD_S3SAVE_PARAMS *AmdS3SaveParamsPtr;
	AMD_INTERFACE_PARAMS AmdInterfaceParams;
	S3_DATA_TYPE S3DataType;

	memset(&AmdInterfaceParams, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdInterfaceParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
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

	S3DataType = S3DataTypeNonVolatile;

	status = OemAgesaSaveS3Info(S3DataType,
				    AmdS3SaveParamsPtr->S3DataBlock.NvStorageSize,
				    AmdS3SaveParamsPtr->S3DataBlock.NvStorage);

	if (AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize != 0) {
		S3DataType = S3DataTypeVolatile;

		status = OemAgesaSaveS3Info(S3DataType,
					    AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize,
					    AmdS3SaveParamsPtr->S3DataBlock.VolatileStorage);
	}

	OemAgesaSaveMtrr();
	AmdReleaseStruct(&AmdInterfaceParams);

	return status;
}
#endif /* #ifndef __PRE_RAM__ */
