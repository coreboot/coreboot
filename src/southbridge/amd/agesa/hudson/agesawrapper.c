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

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
#error Use of GetHeapBase() is incorrect or at least suspicious
#endif

AGESA_STATUS agesawrapper_fchs3earlyrestore (void)
{
	FCH_DATA_BLOCK      FchParams;
	AMD_CONFIG_PARAMS StdHeader;

	StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	StdHeader.HeapBasePtr = (uintptr_t) GetHeapBase() + 0x10;
	StdHeader.AltImageBasePtr = 0;
	StdHeader.CalloutPtr = &GetBiosCallout;
	StdHeader.Func = 0;
	StdHeader.ImageBasePtr = 0;

	FchParams.StdHeader = &StdHeader;
	s3_resume_init_data(&FchParams);
	FchInitS3EarlyRestore(&FchParams);

	return  AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_fchs3laterestore (void)
{
	FCH_DATA_BLOCK      FchParams;
	AMD_CONFIG_PARAMS StdHeader;

	StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	StdHeader.HeapBasePtr = (uintptr_t) GetHeapBase() + 0x10;
	StdHeader.AltImageBasePtr = 0;
	StdHeader.CalloutPtr = &GetBiosCallout;
	StdHeader.Func = 0;
	StdHeader.ImageBasePtr = 0;

	FchParams.StdHeader = &StdHeader;
	s3_resume_init_data(&FchParams);
	FchInitS3LateRestore(&FchParams);

	return AGESA_SUCCESS;
}
