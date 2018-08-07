/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

#include <cbfs.h>
#include <spd_bin.h>
#include <string.h>

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/dimmSpd.h>

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_PI)
#if IS_ENABLED(CONFIG_ARCH_ROMSTAGE_X86_64) || \
	IS_ENABLED(CONFIG_ARCH_RAMSTAGE_X86_64)
#error "FIXME: CALLOUT_ENTRY is UINT32 Data, not UINT Data"
#endif
#endif

AGESA_STATUS GetBiosCallout (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS status;
	UINTN i;

	if (HAS_LEGACY_WRAPPER || ENV_RAMSTAGE) {
		/* One HeapManager serves them all. */
		status = HeapManagerCallout(Func, Data, ConfigPtr);
		if (status != AGESA_UNSUPPORTED)
			return status;
	}

#if HAS_AGESA_FCH_OEM_CALLOUT
	if (!HAS_LEGACY_WRAPPER && Func == AGESA_FCH_OEM_CALLOUT) {
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

	return BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);
}

AGESA_STATUS agesa_NoopUnsupported (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_NoopSuccess (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_EmptyIdsInitData (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	IDS_NV_ITEM *IdsPtr = ((IDS_CALLOUT_STRUCT *) ConfigPtr)->IdsNvPtr;
	if (Data == IDS_CALLOUT_INIT)
		IdsPtr[0].IdsNvValue = IdsPtr[0].IdsNvId = 0xffff;
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_Reset (UINT32 Func, UINTN Data, VOID *ConfigPtr)
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
		LibAmdIoWrite (AccessWidth8, 0xCf9, &Value, StdHeader);
		break;

	default:
		break;
	}

	Status = 0;
	return Status;
}

AGESA_STATUS agesa_RunFuncOnAp (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(AP_EXE_PARAMS));

	if (HAS_LEGACY_WRAPPER) {
		ApExeParams.StdHeader.AltImageBasePtr = 0;
		ApExeParams.StdHeader.CalloutPtr = &GetBiosCallout;
		ApExeParams.StdHeader.Func = 0;
		ApExeParams.StdHeader.ImageBasePtr = 0;
	} else {
		memcpy(&ApExeParams.StdHeader, StdHeader, sizeof(*StdHeader));
	}

	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

#if HAS_LEGACY_WRAPPER
	status = AmdLateRunApTask(&ApExeParams);
#else
	status = module_dispatch(AMD_LATE_RUN_AP_TASK, &ApExeParams.StdHeader);
#endif

	ASSERT(status == AGESA_SUCCESS);
	return status;
}

#if defined(AGESA_GNB_GFX_GET_VBIOS_IMAGE)
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData, VOID *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO  *pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_boot_map_with_leak(
			"pci"CONFIG_VGA_BIOS_ID".rom",
			CBFS_TYPE_OPTIONROM, NULL);
	/* printk(BIOS_DEBUG, "IMGptr=%x\n", pVbiosImageInfo->ImagePtr); */
	return pVbiosImageInfo->ImagePtr == NULL ? AGESA_WARNING : AGESA_SUCCESS;
}
#endif

AGESA_STATUS agesa_ReadSpd (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;
#ifdef __PRE_RAM__
	Status = AmdMemoryReadSPD (Func, Data, ConfigPtr);
#endif
	return Status;
}

AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;
#ifdef __PRE_RAM__
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;
	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_ddr3_spd_from_cbfs((u8*)info->Buffer, 0) < 0)
		die("No SPD data\n");

	Status = AGESA_SUCCESS;
#endif
	return Status;
}

#if HAS_AGESA_FCH_OEM_CALLOUT
void agesa_fch_oem_config(uintptr_t Data, AMD_CONFIG_PARAMS *StdHeader)
{
	/* FIXME: CAR_GLOBAL needed here to pass sysinfo. */
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
