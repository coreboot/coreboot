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
#include <spd_cache.h>

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "agesawrapper.h"
#include "BiosCallOuts.h"
#include "dimmSpd.h"

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN i;

	for (i = 0; i < BiosCalloutsLen; i++) {
		if (BiosCallouts[i].CalloutName == Func)
			break;
	}
	if(i >= BiosCalloutsLen)
		return AGESA_UNSUPPORTED;

	return BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);
}

AGESA_STATUS agesa_NoopUnsupported (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_NoopSuccess (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_EmptyIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	IDS_NV_ITEM *IdsPtr = ((IDS_CALLOUT_STRUCT *) ConfigPtr)->IdsNvPtr;
	if (Data == IDS_CALLOUT_INIT)
		IdsPtr[0].IdsNvValue = IdsPtr[0].IdsNvId = 0xffff;
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_Reset (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS        Status;
	UINT8                 Value;
	UINTN               ResetType;
	AMD_CONFIG_PARAMS   *StdHeader;

	ResetType = Data;
	StdHeader = ConfigPtr;

	//
	// Perform the RESET based upon the ResetType. In case of
	// WARM_RESET_WHENVER and COLD_RESET_WHENEVER, the request will go to
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

AGESA_STATUS agesa_RunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS        Status;

	Status = agesawrapper_amdlaterunaptask (Func, Data, ConfigPtr);
	return Status;
}

AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINT32 FchData, VOID *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO  *pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_boot_map_with_leak(
			"pci"CONFIG_VGA_BIOS_ID".rom",
			CBFS_TYPE_OPTIONROM, NULL);
	printk(BIOS_DEBUG, "agesa_GfxGetVbiosImage: IMGptr=%p\n", pVbiosImageInfo->ImagePtr);
	return (pVbiosImageInfo->ImagePtr ? AGESA_SUCCESS : AGESA_WARNING);
}

AGESA_STATUS agesa_ReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;
#ifdef __PRE_RAM__
	Status = AmdMemoryReadSPD (Func, Data, ConfigPtr);
#endif
	return Status;
}

AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINT32 Data, VOID *ConfigPtr)
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
	if (read_spd_from_cbfs((u8*)info->Buffer, 0) < 0)
		die("No SPD data\n");

	Status = AGESA_SUCCESS;
#endif
	return Status;
}
