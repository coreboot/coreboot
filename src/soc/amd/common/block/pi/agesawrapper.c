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

#include <arch/early_variables.h>
#include <arch/acpi.h>
#include <cpu/x86/mtrr.h>
#include <cbfs.h>
#include <cbmem.h>
#include <delay.h>
#include <rules.h>
#include <rmodule.h>
#include <string.h>
#include <timestamp.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/BiosCallOuts.h>

void __attribute__((weak)) SetMemParams(AMD_POST_PARAMS *PostParams) {}
void __attribute__((weak)) OemPostParams(AMD_POST_PARAMS *PostParams) {}

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

static AGESA_STATUS agesawrapper_readeventlog(UINT8 HeapStatus)
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

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status;
	AMD_RESET_PARAMS ResetParams;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_RESET,
		.AllocationMethod = ByHost,
		.NewStructSize = sizeof(AMD_RESET_PARAMS),
		.NewStructPtr = &ResetParams,
		.StdHeader.CalloutPtr = &GetBiosCallout
	};
	AmdCreateStruct(&AmdParamStruct);
	SetFchResetParams(&ResetParams.FchInterface);

	timestamp_add_now(TS_AGESA_INIT_RESET_START);
	status = AmdInitReset(&ResetParams);
	timestamp_add_now(TS_AGESA_INIT_RESET_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_EARLY_PARAMS *EarlyParams;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_EARLY,
		.AllocationMethod = PreMemHeap,
		.StdHeader.CalloutPtr = &GetBiosCallout,
	};

	AmdCreateStruct(&AmdParamStruct);

	EarlyParams = (AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly(EarlyParams);

	EarlyParams->GnbConfig.PsppPolicy = PsppDisabled;

	timestamp_add_now(TS_AGESA_INIT_EARLY_START);
	status = AmdInitEarly(EarlyParams);
	timestamp_add_now(TS_AGESA_INIT_EARLY_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

static void print_init_post_settings(AMD_POST_PARAMS *parms)
{
	u64 syslimit, bottomio, uma_size, uma_start;
	const char *mode;

	switch (parms->MemConfig.UmaMode) {
	case UMA_AUTO:
		mode = "UMA_AUTO";
		break;
	case UMA_SPECIFIED:
		mode = "UMA_SPECIFIED";
		break;
	case UMA_NONE:
		mode = "UMA_NONE";
		break;
	default:
		mode = "unknown!";
		break;
	}

	syslimit = (u64)parms->MemConfig.SysLimit * 64 * KiB;
	bottomio = (u64)parms->MemConfig.BottomIo * 64 * KiB;

	uma_size = (u64)parms->MemConfig.UmaSize * 64 * KiB;
	uma_start = (u64)parms->MemConfig.UmaBase * 64 * KiB;

	printk(BIOS_SPEW, "AGESA set: umamode %s\n", mode);
	printk(BIOS_SPEW, "         : syslimit 0x%llx, bottomio 0x%08llx\n",
					syslimit, bottomio);
	printk(BIOS_SPEW, "         : uma size %lluMB, uma start 0x%08llx\n",
					uma_size / MiB, uma_start);
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_POST,
		.AllocationMethod = PreMemHeap,
		.StdHeader.CalloutPtr = &GetBiosCallout,
	};
	AMD_POST_PARAMS *PostParams;

	AmdCreateStruct(&AmdParamStruct);

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
	status = AmdInitPost(PostParams);
	timestamp_add_now(TS_AGESA_INIT_POST_DONE);

	/*
	 * If UMA is enabled we currently have it below TOP_MEM as well.
	 * UMA may or may not be cacheable, so Sub4GCacheTop could be
	 * higher than UmaBase. With UMA_NONE we see UmaBase==0.
	 */
	uintptr_t top;
	if (PostParams->MemConfig.UmaBase)
		top = PostParams->MemConfig.UmaBase << 16;
	else
		top = PostParams->MemConfig.Sub4GCacheTop;
	backup_top_of_low_cacheable(top);

	print_init_post_settings(PostParams);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(PostParams->StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_ENV,
		.AllocationMethod = PostMemDram,
		.StdHeader.CalloutPtr = &GetBiosCallout,
	};
	AMD_ENV_PARAMS *EnvParams;

	status = AmdCreateStruct(&AmdParamStruct);

	EnvParams = (AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr;
	SetFchEnvParams(&EnvParams->FchInterface);
	SetNbEnvParams(&EnvParams->GnbEnvConfiguration);

	timestamp_add_now(TS_AGESA_INIT_ENV_START);
	status = AmdInitEnv(EnvParams);
	timestamp_add_now(TS_AGESA_INIT_ENV_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(EnvParams->StdHeader.HeapStatus);

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
	case PICK_IVRS:
		return AcpiIvrs;
	case PICK_CRAT:
		return AcpiCrat;
	default:
		return NULL;
	}
}

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_MID,
		.AllocationMethod = PostMemDram,
		.StdHeader.CalloutPtr = &GetBiosCallout,
	};
	AMD_MID_PARAMS *MidParams;

	/* Enable MMIO on AMD CPU Address Map Controller */
	amd_initcpuio();

	AmdCreateStruct(&AmdParamStruct);

	MidParams = (AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr;
	SetFchMidParams(&MidParams->FchInterface);
	SetNbMidParams(&MidParams->GnbMidConfiguration);

	timestamp_add_now(TS_AGESA_INIT_MID_START);
	status = AmdInitMid(MidParams);
	timestamp_add_now(TS_AGESA_INIT_MID_DONE);

	if (status != AGESA_SUCCESS)
		agesawrapper_readeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct = {
		.AgesaFunctionName = AMD_INIT_LATE,
		.AllocationMethod = PostMemDram,
		.StdHeader.CalloutPtr = &GetBiosCallout,
	};
	AMD_LATE_PARAMS *LateParams;

	/*
	 * NOTE: if not call amdcreatestruct, the initializer
	 * (AmdInitLateInitializer) would not be called.
	 */
	AmdCreateStruct(&AmdParamStruct);
	LateParams = (AMD_LATE_PARAMS *)AmdParamStruct.NewStructPtr;

	timestamp_add_now(TS_AGESA_INIT_LATE_START);
	Status = AmdInitLate(LateParams);
	timestamp_add_now(TS_AGESA_INIT_LATE_DONE);

	if (Status != AGESA_SUCCESS) {
		agesawrapper_readeventlog(LateParams->StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	DmiTable = LateParams->DmiTable;
	AcpiPstate = LateParams->AcpiPState;

	AcpiWheaMce = LateParams->AcpiWheaMce;
	AcpiWheaCmc = LateParams->AcpiWheaCmc;
	AcpiAlib = LateParams->AcpiAlib;
	AcpiIvrs = LateParams->AcpiIvrs;
	AcpiCrat = LateParams->AcpiCrat;

	printk(BIOS_DEBUG, "DmiTable:%p, AcpiPstatein: %p, AcpiSrat:%p,"
	       "AcpiSlit:%p, Mce:%p, Cmc:%p,"
	       "Alib:%p, AcpiIvrs:%p in %s\n",
	       DmiTable, AcpiPstate, AcpiSrat,
	       AcpiSlit, AcpiWheaMce, AcpiWheaCmc,
	       AcpiAlib, AcpiIvrs, __func__);

	/* AmdReleaseStruct (&AmdParamStruct); */
	return Status;
}

AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINTN Data,
				VOID *ConfigPtr)
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

	Status = AmdLateRunApTask(&ApExeParams);
	if (Status != AGESA_SUCCESS) {
		/* agesawrapper_readeventlog(); */
		ASSERT(Status == AGESA_SUCCESS);
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

static int agesa_locate_stage_file_early(const char *name,
					struct region_device *rdev)
{
	const size_t metadata_sz = sizeof(struct cbfs_stage);

	if (agesa_locate_file(name, rdev, CBFS_TYPE_STAGE))
		return -1;

	/* Peel off the cbfs stage metadata. */
	return rdev_chain(rdev, rdev, metadata_sz,
			region_device_sz(rdev) - metadata_sz);
}

static int agesa_locate_stage_file_ramstage(const char *name,
						struct region_device *rdev)
{
	struct prog prog = PROG_INIT(PROG_REFCODE, name);
	struct rmod_stage_load rmod_agesa = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

	if (prog_locate(&prog))
		return -1;
	if (rmodule_stage_load(&rmod_agesa) < 0)
		return -1;

	return rdev_chain(rdev, prog_rdev(&prog), 0,
		region_device_sz(prog_rdev(&prog)));
}

static int agesa_locate_stage_file(const char *name, struct region_device *rdev)
{
	if (!ENV_RAMSTAGE || !IS_ENABLED(CONFIG_AGESA_SPLIT_MEMORY_FILES))
		return agesa_locate_stage_file_early(name, rdev);
	return agesa_locate_stage_file_ramstage(name, rdev);
}

static const char *get_agesa_cbfs_name(void)
{
	if (!IS_ENABLED(CONFIG_AGESA_SPLIT_MEMORY_FILES))
		return CONFIG_AGESA_CBFS_NAME;
	if (!ENV_RAMSTAGE)
		return CONFIG_AGESA_PRE_MEMORY_CBFS_NAME;
	return CONFIG_AGESA_POST_MEMORY_CBFS_NAME;
}

const void *agesawrapper_locate_module(const CHAR8 name[8])
{
	const void *agesa;
	const AMD_IMAGE_HEADER *image;
	struct region_device rdev;
	size_t file_size;
	const char *fname;
	int ret;

	fname = get_agesa_cbfs_name();

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
