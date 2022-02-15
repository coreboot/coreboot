/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <timestamp.h>
#include <amdblocks/biosram.h>
#include <amdblocks/s3_resume.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/BiosCallOuts.h>
#include <amdblocks/ioapic.h>
#include <soc/pci_devs.h>
#include <soc/northbridge.h>
#include <soc/cpu.h>
#include <string.h>

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

static AGESA_STATUS amd_create_struct(AMD_INTERFACE_PARAMS *aip,
	AGESA_STRUCT_NAME func, void *buf, size_t len)
{
	AGESA_STATUS status;

	/* Should clone entire StdHeader here. */
	memset(aip, 0, sizeof(*aip));
	aip->StdHeader.CalloutPtr = &GetBiosCallout;

	/* If we provide the buffer, API expects it to have
	   StdHeader already filled. */
	if (buf != NULL && len >= sizeof(aip->StdHeader)) {
		memcpy(buf, &aip->StdHeader, sizeof(aip->StdHeader));
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
		printk(BIOS_ERR, "AmdCreateStruct() for 0x%x returned 0x%x. "
				"Proper system initialization may not be possible.\n",
				aip->AgesaFunctionName, status);
	}

	if (!aip->NewStructPtr)
		die("No AGESA structure created");

	return status;
}

static AGESA_STATUS amd_release_struct(AMD_INTERFACE_PARAMS *aip)
{
	return module_dispatch(AMD_RELEASE_STRUCT, &aip->StdHeader);
}

static AGESA_STATUS amd_init_reset(AMD_RESET_PARAMS *ResetParams)
{
	AGESA_STATUS status;

	SetFchResetParams(&ResetParams->FchInterface);

	timestamp_add_now(TS_AGESA_INIT_RESET_START);
	status = amd_dispatch(ResetParams);
	timestamp_add_now(TS_AGESA_INIT_RESET_END);

	return status;
}

static AGESA_STATUS amd_init_early(AMD_EARLY_PARAMS *EarlyParams)
{
	AGESA_STATUS status;

	soc_customize_init_early(EarlyParams);
	OemCustomizeInitEarly(EarlyParams);

	timestamp_add_now(TS_AGESA_INIT_EARLY_START);
	status = amd_dispatch(EarlyParams);
	timestamp_add_now(TS_AGESA_INIT_EARLY_END);

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

static AGESA_STATUS amd_init_post(AMD_POST_PARAMS *PostParams)
{
	AGESA_STATUS status;

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
	timestamp_add_now(TS_AGESA_INIT_POST_END);

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

	return status;
}

static AGESA_STATUS amd_init_env(AMD_ENV_PARAMS *EnvParams)
{
	AGESA_STATUS status;

	SetFchEnvParams(&EnvParams->FchInterface);
	SetNbEnvParams(&EnvParams->GnbEnvConfiguration);

	timestamp_add_now(TS_AGESA_INIT_ENV_START);
	status = amd_dispatch(EnvParams);
	timestamp_add_now(TS_AGESA_INIT_ENV_END);

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

static AGESA_STATUS amd_init_mid(AMD_MID_PARAMS *MidParams)
{
	AGESA_STATUS status;

	/* Enable MMIO on AMD CPU Address Map Controller */
	amd_initcpuio();

	SetFchMidParams(&MidParams->FchInterface);
	SetNbMidParams(&MidParams->GnbMidConfiguration);

	timestamp_add_now(TS_AGESA_INIT_MID_START);
	status = amd_dispatch(MidParams);
	timestamp_add_now(TS_AGESA_INIT_MID_END);

	return status;
}

static AGESA_STATUS amd_init_late(AMD_LATE_PARAMS *LateParams)
{
	AGESA_STATUS Status;

	const struct device *dev = pcidev_path_on_root(IOMMU_DEVFN);
	if (dev && dev->enabled) {
		LateParams->GnbLateConfiguration.GnbIoapicId = GNB_IOAPIC_ID;
		LateParams->GnbLateConfiguration.FchIoapicId = FCH_IOAPIC_ID;
	}

	timestamp_add_now(TS_AGESA_INIT_LATE_START);
	Status = amd_dispatch(LateParams);
	timestamp_add_now(TS_AGESA_INIT_LATE_END);

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

	return Status;
}

static AGESA_STATUS amd_init_rtb(AMD_RTB_PARAMS *RtbParams)
{
	AGESA_STATUS Status;

	timestamp_add_now(TS_AGESA_INIT_RTB_START);
	Status = amd_dispatch(RtbParams);
	timestamp_add_now(TS_AGESA_INIT_RTB_END);

	if (Status != AGESA_SUCCESS)
		return Status;

	if (OemS3Save(&RtbParams->S3DataBlock) != AGESA_SUCCESS)
		printk(BIOS_ERR, "S3 data not saved, resuming impossible\n");

	return Status;
}

static AGESA_STATUS amd_init_resume(AMD_RESUME_PARAMS *InitResumeParams)
{
	AGESA_STATUS status;

	OemInitResume(&InitResumeParams->S3DataBlock);

	timestamp_add_now(TS_AGESA_INIT_RESUME_START);
	status = amd_dispatch(InitResumeParams);
	timestamp_add_now(TS_AGESA_INIT_RESUME_END);

	return status;
}

static AGESA_STATUS amd_s3late_restore(AMD_S3LATE_PARAMS *S3LateParams)
{
	AGESA_STATUS Status;

	amd_initcpuio();

	OemS3LateRestore(&S3LateParams->S3DataBlock);

	timestamp_add_now(TS_AGESA_S3_LATE_START);
	Status = amd_dispatch(S3LateParams);
	timestamp_add_now(TS_AGESA_S3_LATE_END);

	return Status;
}

static AGESA_STATUS amd_s3final_restore(AMD_S3FINAL_PARAMS *S3FinalParams)
{
	AGESA_STATUS Status;

	OemS3LateRestore(&S3FinalParams->S3DataBlock);

	timestamp_add_now(TS_AGESA_S3_FINAL_START);
	Status = amd_dispatch(S3FinalParams);
	timestamp_add_now(TS_AGESA_S3_FINAL_END);

	return Status;
}

static AGESA_STATUS romstage_dispatch(AMD_CONFIG_PARAMS *StdHeader)
{
	void *Params = StdHeader;

	switch (StdHeader->Func) {
	case AMD_INIT_RESET:
		return amd_init_reset(Params);
	case AMD_INIT_EARLY:
		return amd_init_early(Params);
	case AMD_INIT_POST:
		return amd_init_post(Params);
	case AMD_INIT_RESUME:
		return amd_init_resume(Params);
	default:
		return AGESA_UNSUPPORTED;
	}
}

static AGESA_STATUS ramstage_dispatch(AMD_CONFIG_PARAMS *StdHeader)
{
	void *Params = StdHeader;

	switch (StdHeader->Func) {
	case AMD_INIT_ENV:
		return amd_init_env(Params);
	case AMD_INIT_MID:
		return amd_init_mid(Params);
	case AMD_INIT_LATE:
		return amd_init_late(Params);
	case AMD_INIT_RTB:
		return amd_init_rtb(Params);
	case AMD_S3LATE_RESTORE:
		return amd_s3late_restore(Params);
	case AMD_S3FINAL_RESTORE:
		return amd_s3final_restore(Params);
	default:
		return AGESA_UNSUPPORTED;
	}

}

AGESA_STATUS agesa_execute_state(AGESA_STRUCT_NAME func)
{
	AGESA_STATUS status = AGESA_UNSUPPORTED;
	AMD_CONFIG_PARAMS template = {};
	AMD_CONFIG_PARAMS *StdHeader = &template;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_INTERFACE_PARAMS *aip = &AmdParamStruct;
	union {
		AMD_RESET_PARAMS ResetParams;
		AMD_S3LATE_PARAMS S3LateParams;
		AMD_S3FINAL_PARAMS S3FinalParams;
	} sp;

	if ((func == AMD_INIT_RESET) || (func == AMD_S3LATE_RESTORE) ||
	    (func == AMD_S3FINAL_RESTORE)) {
		memset(&sp, 0, sizeof(sp));
		amd_create_struct(aip, func, &sp, sizeof(sp));
	} else {
		amd_create_struct(aip, func, NULL, 0);
	}

	StdHeader = aip->NewStructPtr;
	StdHeader->Func = func;

	if (ENV_ROMSTAGE)
		status = romstage_dispatch(StdHeader);
	if (ENV_RAMSTAGE)
		status = ramstage_dispatch(StdHeader);

	amd_release_struct(aip);
	return status;
}
