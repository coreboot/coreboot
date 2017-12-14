/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2017 Advanced Micro Devices, Inc.
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

#include <amdblocks/agesawrapper.h>
#include <arch/early_variables.h>
#include <cbfs.h>
#include <cbmem.h>
#include <delay.h>
#include <cpu/x86/mtrr.h>
#include <amdblocks/BiosCallOuts.h>
#include <string.h>
#include <timestamp.h>

void __attribute__((weak)) SetMemParams(AMD_POST_PARAMS *PostParams) {}
void __attribute__((weak)) OemPostParams(AMD_POST_PARAMS *PostParams) {}

#ifndef __PRE_RAM__
/* ACPI table pointers returned by AmdInitLate */
static void *DmiTable;
static void *AcpiPstate;
static void *AcpiSrat;
static void *AcpiSlit;

static void *AcpiWheaMce;
static void *AcpiWheaCmc;
static void *AcpiAlib;
static void *AcpiIvrs;
static void *AcpiCrat;
#endif /* #ifndef __PRE_RAM__ */

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));
	memset(&AmdResetParams, 0, sizeof(AmdResetParams));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);

	AmdResetParams.FchInterface.Xhci0Enable = IS_ENABLED(CONFIG_STONEYRIDGE_XHCI_ENABLE);

	AmdResetParams.FchInterface.SataEnable = !((CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3));
	AmdResetParams.FchInterface.IdeEnable = (CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3);

	timestamp_add_now(TS_AGESA_INIT_RESET_START);
	status = AmdInitReset(&AmdResetParams);
	timestamp_add_now(TS_AGESA_INIT_RESET_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS     *AmdEarlyParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));

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

	timestamp_add_now(TS_AGESA_INIT_EARLY_START);
	status = AmdInitEarly ((AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr);
	timestamp_add_now(TS_AGESA_INIT_EARLY_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS  AmdParamStruct;
	AMD_POST_PARAMS       *PostParams;

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);
	PostParams = (AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr;

	PostParams->MemConfig.UmaMode = CONFIG_GFXUMA ? UMA_AUTO : UMA_NONE;
	PostParams->MemConfig.UmaSize = 0;
	PostParams->MemConfig.BottomIo = (UINT16)
					 (CONFIG_BOTTOMIO_POSITION >> 24);

	SetMemParams(PostParams);
	OemPostParams(PostParams);
	printk(BIOS_SPEW, "DRAM clear on reset: %s\n",
		(PostParams->MemConfig.EnableMemClr == FALSE) ? "Keep" :
		(PostParams->MemConfig.EnableMemClr == TRUE) ? "Clear" :
		"unknown"
	);

	timestamp_add_now(TS_AGESA_INIT_POST_START);
	status = AmdInitPost (PostParams);
	timestamp_add_now(TS_AGESA_INIT_POST_DONE);

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

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(PostParams->StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS       *EnvParam;

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct (&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr;

	EnvParam->FchInterface.AzaliaController = AzEnable;
	EnvParam->FchInterface.SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
	EnvParam->FchInterface.SataEnable = !((CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3));
	EnvParam->FchInterface.IdeEnable = (CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3);
	EnvParam->FchInterface.SataIdeMode = (CONFIG_STONEYRIDGE_SATA_MODE == 3);
	EnvParam->GnbEnvConfiguration.IommuSupport = FALSE;

	timestamp_add_now(TS_AGESA_INIT_ENV_START);
	status = AmdInitEnv (EnvParam);
	timestamp_add_now(TS_AGESA_INIT_ENV_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(EnvParam->StdHeader.HeapStatus);
	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	*/

	return status;
}

#ifndef __PRE_RAM__
VOID* agesawrapper_getlateinitptr (int pick)
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
	case PICK_IVRS:
		return AcpiIvrs;
	case PICK_CRAT:
		return AcpiCrat;
	default:
		return NULL;
	}
}
#endif /* #ifndef __PRE_RAM__ */

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_MID_PARAMS *MidParam;

	/* Enable MMIO on AMD CPU Address Map Controller */
	amd_initcpuio ();

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));

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
	MidParam->FchInterface.SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
	MidParam->FchInterface.SataEnable = !((CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3));
	MidParam->FchInterface.IdeEnable = (CONFIG_STONEYRIDGE_SATA_MODE == 0) || (CONFIG_STONEYRIDGE_SATA_MODE == 3);
	MidParam->FchInterface.SataIdeMode = (CONFIG_STONEYRIDGE_SATA_MODE == 3);

	timestamp_add_now(TS_AGESA_INIT_MID_START);
	status = AmdInitMid ((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
	timestamp_add_now(TS_AGESA_INIT_MID_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

#ifndef __PRE_RAM__
AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParams;

	memset(&AmdParamStruct, 0, sizeof(AmdParamStruct));

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

	timestamp_add_now(TS_AGESA_INIT_LATE_START);
	Status = AmdInitLate(AmdLateParams);
	timestamp_add_now(TS_AGESA_INIT_LATE_DONE);

	if (Status != AGESA_SUCCESS) {
		agesawrapper_readeventlog(AmdLateParams->StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	DmiTable    = AmdLateParams->DmiTable;
	AcpiPstate  = AmdLateParams->AcpiPState;

	AcpiWheaMce = AmdLateParams->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParams->AcpiWheaCmc;
	AcpiAlib    = AmdLateParams->AcpiAlib;
	AcpiIvrs    = AmdLateParams->AcpiIvrs;
	AcpiCrat    = AmdLateParams->AcpiCrat;

	printk(BIOS_DEBUG, "DmiTable:%x, AcpiPstatein: %x, AcpiSrat:%x,"
	       "AcpiSlit:%x, Mce:%x, Cmc:%x,"
	       "Alib:%x, AcpiIvrs:%x in %s\n",
	       (unsigned int)DmiTable, (unsigned int)AcpiPstate, (unsigned int)AcpiSrat,
	       (unsigned int)AcpiSlit, (unsigned int)AcpiWheaMce, (unsigned int)AcpiWheaCmc,
	       (unsigned int)AcpiAlib, (unsigned int)AcpiIvrs, __func__);

	/* AmdReleaseStruct (&AmdParamStruct); */
	return Status;
}
#endif /* #ifndef __PRE_RAM__ */

AGESA_STATUS agesawrapper_amdlaterunaptask (
	UINT32 Func,
	UINTN Data,
	VOID *ConfigPtr
	)
{
	AGESA_STATUS Status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(ApExeParams));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = &GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	Status = AmdLateRunApTask (&ApExeParams);
	if (Status != AGESA_SUCCESS) {
		/* agesawrapper_readeventlog(); */
		ASSERT(Status == AGESA_SUCCESS);
	}

	return Status;
}

AGESA_STATUS agesawrapper_readeventlog(UINT8 HeapStatus)
{
	AGESA_STATUS Status;
	EVENT_PARAMS AmdEventParams = {
		.StdHeader.CalloutPtr = &GetBiosCallout,
		.StdHeader.HeapStatus = HeapStatus,
	};

	Status = AmdReadEventLog(&AmdEventParams);
	if (AmdEventParams.EventClass)
		printk(BIOS_DEBUG, "AGESA Event Log:\n");

	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG, "  Class = %x, Info = %x,"
				" Param1 = 0x%x, Param2 = 0x%x"
				" Param3 = 0x%x, Param4 = 0x%x\n",
				(u32)AmdEventParams.EventClass,
				(u32)AmdEventParams.EventInfo,
				(u32)AmdEventParams.DataParam1,
				(u32)AmdEventParams.DataParam2,
				(u32)AmdEventParams.DataParam3,
				(u32)AmdEventParams.DataParam4);
		Status = AmdReadEventLog(&AmdEventParams);
	}

	return Status;
}

static int agesa_locate_file(const char *name, struct region_device *rdev,
				uint32_t type)
{
	struct cbfsf fh;

	if (cbfs_boot_locate(&fh, name, &type))
		return -1;

	cbfs_file_data(rdev, &fh);
	return 0;
}

static int agesa_locate_raw_file(const char *name, struct region_device *rdev)
{
	return agesa_locate_file(name, rdev, CBFS_TYPE_RAW);
}

static int agesa_locate_stage_file(const char *name, struct region_device *rdev)
{
	const size_t metadata_sz = sizeof(struct cbfs_stage);

	if (agesa_locate_file(name, rdev, CBFS_TYPE_STAGE))
		return -1;

	/* Peel off the cbfs stage metadata. */
	return rdev_chain(rdev, rdev, metadata_sz,
			region_device_sz(rdev) - metadata_sz);
}

const void *agesawrapper_locate_module (const CHAR8 name[8])
{
	const void* agesa;
	const AMD_IMAGE_HEADER* image;
	struct region_device rdev;
	size_t file_size;
	const char *fname = CONFIG_AGESA_CBFS_NAME;
	int ret;

	if (IS_ENABLED(CONFIG_AGESA_BINARY_PI_AS_STAGE))
		ret = agesa_locate_stage_file(fname, &rdev);
	else
		ret = agesa_locate_raw_file(fname, &rdev);

	if (ret)
		return NULL;

	file_size = region_device_sz(&rdev);

	/* Assume boot device is memory mapped so the mapping can leak. */
	assert(IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED));

	agesa = rdev_mmap_full(&rdev);

	if (!agesa)
		return NULL;

	image =  LibAmdLocateImage(agesa, agesa + file_size, 4096, name);

	if (!image)
		return NULL;

	return (AMD_MODULE_HEADER *)image->ModuleInfoOffset;
}

static MODULE_ENTRY agesa_dispatcher CAR_GLOBAL;

MODULE_ENTRY agesa_get_dispatcher(void)
{
	const AMD_MODULE_HEADER *module;
	static const CHAR8 id[8] = AGESA_ID;
	MODULE_ENTRY val = car_get_var(agesa_dispatcher);

	if (val != NULL)
		return val;

	module = agesawrapper_locate_module(id);
	if (!module)
		return NULL;

	val = module->ModuleDispatcher;
	car_set_var(agesa_dispatcher, val);

	return val;
}
