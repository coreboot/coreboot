/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2014 Advanced Micro Devices, Inc.
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

#include <AGESA.h>
#include <cbfs.h>
#include <cbmem.h>
#include <compiler.h>
#include <delay.h>
#include <cpu/x86/mtrr.h>
#include <FchPlatform.h>
#include <heapManager.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>

void __weak OemPostParams(AMD_POST_PARAMS *PostParams) {}

#define FILECODE UNASSIGNED_FILE_FILECODE

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	LibAmdMemFill (&AmdResetParams,
		       0,
		       sizeof(AMD_RESET_PARAMS),
		       &(AmdResetParams.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);

	AmdResetParams.FchInterface.Xhci0Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
	if (IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_PI_BOLTON))
		AmdResetParams.FchInterface.Xhci1Enable = TRUE;

	AmdResetParams.FchInterface.SataEnable = !((CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3));
	AmdResetParams.FchInterface.IdeEnable = (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);

	status = AmdInitReset(&AmdResetParams);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS     *AmdEarlyParamsPtr;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly (AmdEarlyParamsPtr);

	AmdEarlyParamsPtr->GnbConfig.PsppPolicy = PsppDisabled;
	status = AmdInitEarly ((AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr);
	/*
	 * init_timer() needs to be called on CZ PI, because AGESA resets the LAPIC reload value
	 * on the AMD_INIT_EARLY call
	 */
	if (IS_ENABLED(CONFIG_CPU_AMD_PI_00660F01))
		init_timer();
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS  AmdParamStruct;
	AMD_POST_PARAMS       *PostParams;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = NULL;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);
	PostParams = (AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr;

	// Do not use IS_ENABLED here.  CONFIG_GFXUMA should always have a value.  Allow
	// the compiler to flag the error if CONFIG_GFXUMA is not set.
	PostParams->MemConfig.UmaMode = CONFIG_GFXUMA ? UMA_AUTO : UMA_NONE;
	PostParams->MemConfig.UmaSize = 0;
	PostParams->MemConfig.BottomIo = (UINT16)
					 (CONFIG_BOTTOMIO_POSITION >> 24);

	OemPostParams(PostParams);

	status = AmdInitPost (PostParams);

	/* If UMA is enabled we currently have it below TOP_MEM as well.
	 * UMA may or may not be cacheable, so Sub4GCacheTop could be
	 * higher than UmaBase. With UMA_NONE we see UmaBase==0. */
	if (PostParams->MemConfig.UmaBase)
		backup_top_of_low_cacheable(PostParams->MemConfig.UmaBase << 16);
	else
		backup_top_of_low_cacheable(PostParams->MemConfig.Sub4GCacheTop);

	printk(
			BIOS_SPEW,
			"setup_uma_memory: umamode %s\n",
			(PostParams->MemConfig.UmaMode == UMA_AUTO) ? "UMA_AUTO" :
			(PostParams->MemConfig.UmaMode == UMA_SPECIFIED) ? "UMA_SPECIFIED" :
			(PostParams->MemConfig.UmaMode == UMA_NONE) ? "UMA_NONE" :
			"unknown"
	);
	printk(
			BIOS_SPEW,
			"setup_uma_memory: syslimit 0x%08llX, bottomio 0x%08lx\n",
			(unsigned long long)(PostParams->MemConfig.SysLimit) << 16,
			(unsigned long)(PostParams->MemConfig.BottomIo) << 16
	);
	printk(
			BIOS_SPEW,
			"setup_uma_memory: uma size %luMB, uma start 0x%08lx\n",
			(unsigned long)(PostParams->MemConfig.UmaSize) >> (20 - 16),
			(unsigned long)(PostParams->MemConfig.UmaBase) << 16
	);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(PostParams->StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS       *EnvParam;

	/* Initialize heap space */
	EmptyHeap();

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct (&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr;

	EnvParam->FchInterface.AzaliaController = AzEnable;
	EnvParam->FchInterface.SataClass = CONFIG_HUDSON_SATA_MODE;
	EnvParam->FchInterface.SataEnable = !((CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3));
	EnvParam->FchInterface.IdeEnable = (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);
	EnvParam->FchInterface.SataIdeMode = (CONFIG_HUDSON_SATA_MODE == 3);

	status = AmdInitEnv (EnvParam);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(EnvParam->StdHeader.HeapStatus);
	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	*/

	return status;
}

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_MID_PARAMS *MidParam;

	/* Enable MMIO on AMD CPU Address Map Controller */
	amd_initcpuio ();

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);
	MidParam = (AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr;

	MidParam->GnbMidConfiguration.iGpuVgaMode = 0;/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	MidParam->GnbMidConfiguration.GnbIoapicAddress = 0xFEC20000;

	MidParam->FchInterface.AzaliaController = AzEnable;
	MidParam->FchInterface.SataClass = CONFIG_HUDSON_SATA_MODE;
	MidParam->FchInterface.SataEnable = !((CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3));
	MidParam->FchInterface.IdeEnable = (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);
	MidParam->FchInterface.SataIdeMode = (CONFIG_HUDSON_SATA_MODE == 3);

	status = AmdInitMid ((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

#ifndef __PRE_RAM__
AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParams;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof(AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	/* NOTE: if not call amdcreatestruct, the initializer(AmdInitLateInitializer) would not be called */
	AmdCreateStruct(&AmdParamStruct);
	AmdLateParams = (AMD_LATE_PARAMS *)AmdParamStruct.NewStructPtr;
	AmdLateParams->GnbLateConfiguration.GnbIoapicId = CONFIG_MAX_CPUS + 1;
	AmdLateParams->GnbLateConfiguration.FchIoapicId = CONFIG_MAX_CPUS;
	Status = AmdInitLate(AmdLateParams);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(AmdLateParams->StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	agesawrapper_setlateinitptr(AmdLateParams);

	/* No AmdReleaseStruct(&AmdParamStruct), we need AmdLateParams later. */
	return Status;
}
#endif /* #ifndef __PRE_RAM__ */

const void *agesawrapper_locate_module (const CHAR8 name[8])
{
	const void *agesa;
	const AMD_IMAGE_HEADER* image;
	const AMD_MODULE_HEADER* module;
	size_t file_size;

	if (IS_ENABLED(CONFIG_VBOOT)) {
		/* Use phys. location in flash and prevent vboot from searching cbmem */
		agesa = (void *)CONFIG_AGESA_BINARY_PI_LOCATION;
		file_size = 0x100000;
	} else {
		agesa = cbfs_boot_map_with_leak((const char *)CONFIG_AGESA_CBFS_NAME,
					CBFS_TYPE_RAW, &file_size);
	}

	if (!agesa)
		return NULL;
	image =  LibAmdLocateImage(agesa, agesa + file_size - 1, 4096, name);
	module = (AMD_MODULE_HEADER*)image->ModuleInfoOffset;

	return module;
}
