/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <bootstate.h>
#include <cbmem.h>
#include <device/device.h>
#include <southbridge_pci_devs.h>
#include <fsp_util.h>
#include "../chip.h"
#include <reset.h>

#ifdef __PRE_RAM__

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX)
static void GetUpdDefaultFromFsp (FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION   *UpdData)
{
	VPD_DATA_REGION *VpdDataRgnPtr;
	UPD_DATA_REGION *UpdDataRgnPtr;
	VpdDataRgnPtr = (VPD_DATA_REGION *)(UINT32)(FspInfo->CfgRegionOffset  + FspInfo->ImageBase);
	UpdDataRgnPtr = (UPD_DATA_REGION *)(UINT32)(VpdDataRgnPtr->PcdUpdRegionOffset + FspInfo->ImageBase);
	memcpy((void *)UpdData, (void *)UpdDataRgnPtr, sizeof(UPD_DATA_REGION));
}

static void ConfigureDefaultUpdData(UPD_DATA_REGION   *UpdData)
{
	UpdData->HTEnable = TRUE;
	UpdData->TurboEnable = FALSE;
	UpdData->MemoryDownEnable = FALSE;
	UpdData->FastBootEnable = CONFIG_ENABLE_FSP_FAST_BOOT;
}
#else	/* IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX) */
const PLATFORM_CONFIG DefaultPlatformConfig = {
	TRUE,  /* Hyperthreading */
	FALSE, /* Turbo Mode */
	FALSE, /* Memory Down */
#if IS_ENABLED(CONFIG_ENABLE_FSP_FAST_BOOT)
	TRUE,  /* Fast Boot */
#else
	FALSE, /* Fast Boot */
#endif	/* CONFIG_ENABLE_FSP_FAST_BOOT */
};
#endif	/* IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX) */

/*
 *
 * Call the FSP to do memory init. The FSP doesn't return to this function.
 * The FSP returns to the romstage_main_continue().
 *
 */
void chipset_fsp_early_init(FSP_INIT_PARAMS *FspInitParams,
		FSP_INFO_HEADER *fsp_ptr)
{
	FSP_INIT_RT_BUFFER *pFspRtBuffer = FspInitParams->RtBufferPtr;
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX)
	UPD_DATA_REGION *fsp_upd_data = pFspRtBuffer->Common.UpdDataRgnPtr;
#else
	MEM_CONFIG MemoryConfig;
	memset((void *)&MemoryConfig, 0, sizeof(MEM_CONFIG));
#endif
	FspInitParams->NvsBufferPtr = NULL;

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX)
	/* Initialize the UPD Data */
	GetUpdDefaultFromFsp (fsp_ptr, fsp_upd_data);
	ConfigureDefaultUpdData(fsp_upd_data);
#else
	pFspRtBuffer->Platform.MemoryConfig = &MemoryConfig;
	pFspRtBuffer->PlatformConfiguration.PlatformConfig = &DefaultPlatformConfig;
#endif

#if IS_ENABLED(CONFIG_ENABLE_FSP_FAST_BOOT)
	/* Find the fastboot cache that was saved in the ROM */
	FspInitParams->NvsBufferPtr = find_and_set_fastboot_cache();
#endif

	pFspRtBuffer->Common.BootMode = 0;
}

/* The FSP returns here after the fsp_early_init call */
void ChipsetFspReturnPoint(EFI_STATUS Status,
		VOID *HobListPtr)
{
	*(void **)CBMEM_FSP_HOB_PTR = HobListPtr;
	if (Status == 0xFFFFFFFF) {
		hard_reset();
	}
	romstage_main_continue(Status, HobListPtr);
}

#endif	/* __PRE_RAM__ */
