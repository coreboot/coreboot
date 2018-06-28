/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2017 Advanced Micro Devices, Inc.
 * Copyright (C) 2018 - 2019 Kyösti Mälkki
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

#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <timestamp.h>
#include <amdblocks/s3_resume.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/BiosCallOuts.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <soc/northbridge.h>
#include <soc/cpu.h>

void __weak SetMemParams(AMD_POST_PARAMS *PostParams) {}
void __weak OemPostParams(AMD_POST_PARAMS *PostParams) {}

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

static AGESA_STATUS module_dispatch(AGESA_STRUCT_NAME func,
	AMD_CONFIG_PARAMS *StdHeader)
{
	MODULE_ENTRY dispatcher = agesa_get_dispatcher();

	if (!dispatcher)
		return AGESA_UNSUPPORTED;

	StdHeader->Func = func;
	return dispatcher(StdHeader);
}

static AGESA_STATUS amd_dispatch(void *Params)
{
	AMD_CONFIG_PARAMS *StdHeader = Params;
	return module_dispatch(StdHeader->Func, StdHeader);
}

AGESA_STATUS amd_late_run_ap_task(AP_EXE_PARAMS *ApExeParams)
{
	AMD_CONFIG_PARAMS *StdHeader = (void *)ApExeParams;
	return module_dispatch(AMD_LATE_RUN_AP_TASK, StdHeader);
}

static void *amd_create_struct(AMD_INTERFACE_PARAMS *aip,
	AGESA_STRUCT_NAME func, void *buf, size_t len)
{
	AMD_CONFIG_PARAMS *StdHeader;
	AGESA_STATUS status;

	/* Should clone entire StdHeader here. */
	memset(aip, 0, sizeof(*aip));
	aip->StdHeader.CalloutPtr = &GetBiosCallout;

	/* If we provide the buffer, API expects it to have
	   StdHeader already filled. */
	if (buf != NULL && len >= sizeof(*StdHeader)) {
		memcpy(buf, &aip->StdHeader, sizeof(*StdHeader));
		aip->AllocationMethod = ByHost;
		aip->NewStructPtr = buf;
		aip->NewStructSize = len;
	} else {
		if (ENV_ROMSTAGE)
			aip->AllocationMethod = PreMemHeap;
		if (ENV_RAMSTAGE)
			aip->AllocationMethod = PostMemDram;
	}

	aip->AgesaFunctionName = func;
	status = module_dispatch(AMD_CREATE_STRUCT, &aip->StdHeader);

	if (status != AGESA_SUCCESS) {
		printk(BIOS_ERR, "Error: AmdCreateStruct() for 0x%x returned 0x%x. "
				"Proper system initialization may not be possible.\n",
				aip->AgesaFunctionName, status);
	}

	if (!aip->NewStructPtr)
		die("No AGESA structure created");

	StdHeader = aip->NewStructPtr;
	StdHeader->Func = aip->AgesaFunctionName;
	return StdHeader;
}

static AGESA_STATUS amd_release_struct(AMD_INTERFACE_PARAMS *aip)
{
	return module_dispatch(AMD_RELEASE_STRUCT, &aip->StdHeader);
}

static AGESA_STATUS amd_init_reset(void)
{
	AGESA_STATUS status;
	AMD_RESET_PARAMS _ResetParams;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	AMD_RESET_PARAMS *ResetParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_RESET, &_ResetParams, sizeof(AMD_RESET_PARAMS));

	SetFchResetParams(&ResetParams->FchInterface);

	timestamp_add_now(TS_AGESA_INIT_RESET_START);
	status = amd_dispatch(ResetParams);
	timestamp_add_now(TS_AGESA_INIT_RESET_DONE);

	amd_release_struct(&AmdParamStruct);
	return status;
}

static AGESA_STATUS amd_init_early(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	AMD_EARLY_PARAMS *EarlyParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_EARLY, NULL, 0);

	soc_customize_init_early(EarlyParams);
	OemCustomizeInitEarly(EarlyParams);

	timestamp_add_now(TS_AGESA_INIT_EARLY_START);
	status = amd_dispatch(EarlyParams);
	timestamp_add_now(TS_AGESA_INIT_EARLY_DONE);

	amd_release_struct(&AmdParamStruct);

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

	syslimit = (u64)(parms->MemConfig.SysLimit + 1) * 64 * KiB - 1;
	bottomio = (u64)parms->MemConfig.BottomIo * 64 * KiB;

	uma_size = (u64)parms->MemConfig.UmaSize * 64 * KiB;
	uma_start = (u64)parms->MemConfig.UmaBase * 64 * KiB;

	printk(BIOS_SPEW, "AGESA set: umamode %s\n", mode);
	printk(BIOS_SPEW, "         : syslimit 0x%llx, bottomio 0x%08llx\n",
					syslimit, bottomio);
	printk(BIOS_SPEW, "         : uma size %lluMB, uma start 0x%08llx\n",
					uma_size / MiB, uma_start);
}

static AGESA_STATUS amd_init_post(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	AMD_POST_PARAMS *PostParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_POST, NULL, 0);

	PostParams->MemConfig.UmaMode = CONFIG(GFXUMA) ? UMA_AUTO : UMA_NONE;
	PostParams->MemConfig.UmaSize = 0;
	PostParams->MemConfig.BottomIo = (uint16_t)
					 (CONFIG_BOTTOMIO_POSITION >> 24);

	SetMemParams(PostParams);
	OemPostParams(PostParams);
	printk(BIOS_SPEW, "DRAM clear on reset: %s\n",
		(PostParams->MemConfig.EnableMemClr == FALSE) ? "Keep" :
		(PostParams->MemConfig.EnableMemClr == TRUE) ? "Clear" :
		"unknown"
	);

	timestamp_add_now(TS_AGESA_INIT_POST_START);
	status = amd_dispatch(PostParams);
	timestamp_add_now(TS_AGESA_INIT_POST_DONE);

	/*
	 * AGESA passes back the base and size of UMA. This is the only
	 * opportunity to get and save these settings to be used in resource
	 * allocation. We also need to allocate the top of low memory.
	 * If UMA is below 4GiB, UMA base is the top of low memory, otherwise
	 * Sub4GCachetop is the top of low memory.
	 * With UMA_NONE we see UmaBase==0.
	 */
	uintptr_t top;
	if (PostParams->MemConfig.UmaBase &&
			(PostParams->MemConfig.UmaBase < ((4ull * GiB) >> 16)))
		top = PostParams->MemConfig.UmaBase << 16;
	else
		top = PostParams->MemConfig.Sub4GCacheTop;
	backup_top_of_low_cacheable(top);

	save_uma_size(PostParams->MemConfig.UmaSize * 64 * KiB);
	save_uma_base((u64)PostParams->MemConfig.UmaBase * 64 * KiB);

	print_init_post_settings(PostParams);

	amd_release_struct(&AmdParamStruct);

	return status;
}

static AGESA_STATUS amd_init_env(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	AMD_ENV_PARAMS *EnvParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_ENV, NULL, 0);

	SetFchEnvParams(&EnvParams->FchInterface);
	SetNbEnvParams(&EnvParams->GnbEnvConfiguration);

	timestamp_add_now(TS_AGESA_INIT_ENV_START);
	status = amd_dispatch(EnvParams);
	timestamp_add_now(TS_AGESA_INIT_ENV_DONE);

	amd_release_struct(&AmdParamStruct);

	return status;
}

void *agesawrapper_getlateinitptr(int pick)
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

static AGESA_STATUS amd_init_mid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	/* Enable MMIO on AMD CPU Address Map Controller */
	amd_initcpuio();

	AMD_MID_PARAMS *MidParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_MID, NULL, 0);

	SetFchMidParams(&MidParams->FchInterface);
	SetNbMidParams(&MidParams->GnbMidConfiguration);

	timestamp_add_now(TS_AGESA_INIT_MID_START);
	status = amd_dispatch(MidParams);
	timestamp_add_now(TS_AGESA_INIT_MID_DONE);

	amd_release_struct(&AmdParamStruct);

	return status;
}

static AGESA_STATUS amd_init_late(void)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	/*
	 * NOTE: if not call amdcreatestruct, the initializer
	 * (AmdInitLateInitializer) would not be called.
	 */
	AMD_LATE_PARAMS *LateParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_LATE, NULL, 0);

	const struct device *dev = pcidev_path_on_root(IOMMU_DEVFN);
	if (dev && dev->enabled) {
		LateParams->GnbLateConfiguration.GnbIoapicId = CONFIG_MAX_CPUS
									+ 1;
		LateParams->GnbLateConfiguration.FchIoapicId = CONFIG_MAX_CPUS;
	}

	timestamp_add_now(TS_AGESA_INIT_LATE_START);
	Status = amd_dispatch(LateParams);
	timestamp_add_now(TS_AGESA_INIT_LATE_DONE);

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

	amd_release_struct(&AmdParamStruct);
	return Status;
}

static AGESA_STATUS amd_init_rtb(void)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	AMD_RTB_PARAMS *RtbParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_RTB, NULL, 0);

	timestamp_add_now(TS_AGESA_INIT_RTB_START);
	Status = amd_dispatch(RtbParams);
	timestamp_add_now(TS_AGESA_INIT_RTB_DONE);

	if (save_s3_info(RtbParams->S3DataBlock.NvStorage,
			 RtbParams->S3DataBlock.NvStorageSize,
			 RtbParams->S3DataBlock.VolatileStorage,
			 RtbParams->S3DataBlock.VolatileStorageSize))
		printk(BIOS_ERR, "S3 data not saved, resuming impossible\n");

	amd_release_struct(&AmdParamStruct);

	return Status;
}

static AGESA_STATUS amd_init_resume(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	size_t nv_size;

	AMD_RESUME_PARAMS *InitResumeParams = amd_create_struct(&AmdParamStruct,
		AMD_INIT_RESUME, NULL, 0);

	get_s3nv_info(&InitResumeParams->S3DataBlock.NvStorage, &nv_size);
	InitResumeParams->S3DataBlock.NvStorageSize = nv_size;

	timestamp_add_now(TS_AGESA_INIT_RESUME_START);
	status = amd_dispatch(InitResumeParams);
	timestamp_add_now(TS_AGESA_INIT_RESUME_DONE);

	amd_release_struct(&AmdParamStruct);

	return status;
}

static AGESA_STATUS amd_s3late_restore(void)
{
	AGESA_STATUS Status;
	AMD_S3LATE_PARAMS _S3LateParams;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	size_t vol_size;

	amd_initcpuio();

	AMD_S3LATE_PARAMS *S3LateParams = amd_create_struct(&AmdParamStruct,
		AMD_S3LATE_RESTORE, &_S3LateParams, sizeof(AMD_S3LATE_PARAMS));

	get_s3vol_info(&S3LateParams->S3DataBlock.VolatileStorage, &vol_size);
	S3LateParams->S3DataBlock.VolatileStorageSize = vol_size;

	timestamp_add_now(TS_AGESA_S3_LATE_START);
	Status = amd_dispatch(S3LateParams);
	timestamp_add_now(TS_AGESA_S3_LATE_DONE);

	amd_release_struct(&AmdParamStruct);

	return Status;
}

static AGESA_STATUS amd_s3final_restore(void)
{
	AGESA_STATUS Status;
	AMD_S3FINAL_PARAMS _S3FinalParams;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	size_t vol_size;

	AMD_S3FINAL_PARAMS *S3FinalParams = amd_create_struct(&AmdParamStruct,
		AMD_S3FINAL_RESTORE, &_S3FinalParams, sizeof(AMD_S3FINAL_PARAMS));

	get_s3vol_info(&S3FinalParams->S3DataBlock.VolatileStorage, &vol_size);
	S3FinalParams->S3DataBlock.VolatileStorageSize = vol_size;

	timestamp_add_now(TS_AGESA_S3_FINAL_START);
	Status = amd_dispatch(S3FinalParams);
	timestamp_add_now(TS_AGESA_S3_FINAL_DONE);

	amd_release_struct(&AmdParamStruct);

	return Status;
}

static AGESA_STATUS romstage_dispatch(AMD_CONFIG_PARAMS *StdHeader)
{
	switch (StdHeader->Func) {
	case AMD_INIT_RESET:
		return amd_init_reset();
	case AMD_INIT_EARLY:
		return amd_init_early();
	case AMD_INIT_POST:
		return amd_init_post();
	case AMD_INIT_RESUME:
		return amd_init_resume();
	default:
		return AGESA_UNSUPPORTED;
	}
}

static AGESA_STATUS ramstage_dispatch(AMD_CONFIG_PARAMS *StdHeader)
{
	switch (StdHeader->Func) {
	case AMD_INIT_ENV:
		return amd_init_env();
	case AMD_INIT_MID:
		return amd_init_mid();
	case AMD_INIT_LATE:
		return amd_init_late();
	case AMD_INIT_RTB:
		return amd_init_rtb();
	case AMD_S3LATE_RESTORE:
		return amd_s3late_restore();
	case AMD_S3FINAL_RESTORE:
		return amd_s3final_restore();
	default:
		return AGESA_UNSUPPORTED;
	}
}

AGESA_STATUS agesa_execute_state(AGESA_STRUCT_NAME func)
{
	AGESA_STATUS status = AGESA_UNSUPPORTED;
	AMD_CONFIG_PARAMS template = {};
	AMD_CONFIG_PARAMS *StdHeader = &template;

	StdHeader->Func = func;

	if (ENV_ROMSTAGE)
		status = romstage_dispatch(StdHeader);
	if (ENV_RAMSTAGE)
		status = ramstage_dispatch(StdHeader);

	return status;
}
