/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <spd_bin.h>
#include <string.h>

#include <AGESA.h>
#include <amdlib.h>
#include "Ids.h"
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/dimmSpd.h>

#if ENV_X86_64 && CONFIG(NORTHBRIDGE_AMD_PI)
#error "FIXME: CALLOUT_ENTRY is UINT32 Data, not UINT Data"
#endif

AGESA_STATUS GetBiosCallout (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS status;
	UINTN i;

	if (ENV_RAMSTAGE) {
		/* One HeapManager serves them all. */
		status = HeapManagerCallout(Func, Data, ConfigPtr);
		if (status != AGESA_UNSUPPORTED)
			return status;
	}

#if HAS_AGESA_FCH_OEM_CALLOUT
	if (Func == AGESA_FCH_OEM_CALLOUT) {
		agesa_fch_oem_config(Data, ConfigPtr);
		return AGESA_SUCCESS;
	}
#endif

	for (i = 0; i < BiosCalloutsLen; i++) {
		if (BiosCallouts[i].CalloutName == Func)
			break;
	}
	if (i >= BiosCalloutsLen)
		return AGESA_UNSUPPORTED;

	return BiosCallouts[i].CalloutPtr(Func, Data, ConfigPtr);
}

AGESA_STATUS agesa_NoopUnsupported(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_NoopSuccess(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_EmptyIdsInitData(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	IDS_NV_ITEM *IdsPtr = ((IDS_CALLOUT_STRUCT *) ConfigPtr)->IdsNvPtr;
	if (Data == IDS_CALLOUT_INIT)
		IdsPtr[0].IdsNvValue = IdsPtr[0].IdsNvId = 0xffff;
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_Reset(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS        Status;
	UINT8                 Value;
	UINTN               ResetType;
	AMD_CONFIG_PARAMS   *StdHeader;

	ResetType = Data;
	StdHeader = ConfigPtr;

	//
	// Perform the RESET based upon the ResetType. In case of
	// WARM_RESET_WHENEVER and COLD_RESET_WHENEVER, the request will go to
	// AmdResetManager. During the critical condition, where reset is required
	// immediately, the reset will be invoked directly by writing 0x04 to port
	// 0xCF9 (Reset Port).
	//
	switch (ResetType) {
	case WARM_RESET_WHENEVER:
	case COLD_RESET_WHENEVER:
		break;

	case WARM_RESET_IMMEDIATELY:
	case COLD_RESET_IMMEDIATELY:
		Value = 0x06;
		LibAmdIoWrite(AccessWidth8, 0xCf9, &Value, StdHeader);
		break;

	default:
		break;
	}

	Status = 0;
	return Status;
}

AGESA_STATUS agesa_RunFuncOnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(AP_EXE_PARAMS));
	memcpy(&ApExeParams.StdHeader, StdHeader, sizeof(*StdHeader));

	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = module_dispatch(AMD_LATE_RUN_AP_TASK, &ApExeParams.StdHeader);

	ASSERT(status == AGESA_SUCCESS);
	return status;
}

#if defined(AGESA_GNB_GFX_GET_VBIOS_IMAGE)
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData, VOID *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO  *pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_map("pci"CONFIG_VGA_BIOS_ID".rom", NULL);
	/* printk(BIOS_DEBUG, "IMGptr=%x\n", pVbiosImageInfo->ImagePtr); */
	return pVbiosImageInfo->ImagePtr == NULL ? AGESA_WARNING : AGESA_SUCCESS;
}
#endif

AGESA_STATUS agesa_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	return AmdMemoryReadSPD(Func, Data, ConfigPtr);
}

AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;

	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_ddr3_spd_from_cbfs((u8*)info->Buffer, 0) < 0)
		die("No SPD data\n");

	return AGESA_SUCCESS;
}

#if HAS_AGESA_FCH_OEM_CALLOUT
void agesa_fch_oem_config(uintptr_t Data, AMD_CONFIG_PARAMS *StdHeader)
{
	struct sysinfo *cb_NA = NULL;

	if (StdHeader->Func == AMD_INIT_RESET) {
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET\n");
		board_FCH_InitReset(cb_NA, (FCH_RESET_DATA_BLOCK *)Data);
	} else if (StdHeader->Func == AMD_INIT_ENV) {
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV\n");
		board_FCH_InitEnv(cb_NA, (FCH_DATA_BLOCK *)Data);
	}
}
#endif
