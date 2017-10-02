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

#include <AGESA.h>
#include <amdlib.h>
#include <Ids.h>
#include <agesawrapper.h>
#include <BiosCallOuts.h>
#include <soc/southbridge.h>

#if ENV_BOOTBLOCK
const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	{ AGESA_DO_RESET,                 agesa_Reset },
	{ AGESA_FCH_OEM_CALLOUT,          agesa_fch_initreset },
	{ AGESA_GNB_PCIE_SLOT_RESET,      agesa_PcieSlotResetControl }
};
#else
const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	/* Required callouts */
	{ AGESA_ALLOCATE_BUFFER,          agesa_AllocateBuffer },
	{ AGESA_DEALLOCATE_BUFFER,        agesa_DeallocateBuffer },
	{ AGESA_DO_RESET,                 agesa_Reset },
	{ AGESA_LOCATE_BUFFER,            agesa_LocateBuffer },
	{ AGESA_READ_SPD,                 agesa_ReadSpd },
	{ AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{ AGESA_RUNFUNC_ON_ALL_APS,       agesa_RunFcnOnAllAps },
	{ AMD_LATE_RUN_AP_TASK,           agesa_LateRunApTask },
	{ AGESA_GNB_PCIE_SLOT_RESET,      agesa_PcieSlotResetControl },
	{ AGESA_WAIT_FOR_ALL_APS,         agesa_WaitForAllApsFinished },
	{ AGESA_IDLE_AN_AP,               agesa_IdleAnAp },

	/* Optional callouts */
	{ AGESA_GET_IDS_INIT_DATA,             agesa_EmptyIdsInitData },
	//AgesaHeapRebase - Hook ID?
	{ AGESA_HOOKBEFORE_DRAM_INIT,          agesa_NoopUnsupported },
	{ AGESA_HOOKBEFORE_DQS_TRAINING,       agesa_NoopUnsupported },
	{ AGESA_EXTERNAL_2D_TRAIN_VREF_CHANGE, agesa_NoopUnsupported },
	{ AGESA_HOOKBEFORE_EXIT_SELF_REF,      agesa_NoopUnsupported },
	{ AGESA_GNB_GFX_GET_VBIOS_IMAGE,       agesa_GfxGetVbiosImage },
	{ AGESA_FCH_OEM_CALLOUT,               agesa_fch_initenv },
	{ AGESA_EXTERNAL_VOLTAGE_ADJUST,       agesa_NoopUnsupported },
	{ AGESA_GNB_PCIE_CLK_REQ,              agesa_NoopUnsupported },

	/* Deprecated */
	{ AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY, agesa_NoopUnsupported},
	{ AGESA_READ_SPD_RECOVERY,             agesa_NoopUnsupported },

};
#endif

const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

AGESA_STATUS GetBiosCallout(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	UINTN i;

	for (i = 0 ; i < BiosCalloutsLen ; i++) {
		if (BiosCallouts[i].CalloutName == Func)
			break;
	}

	if (i >= BiosCalloutsLen) {
		printk(BIOS_ERR, "ERROR: AGESA Callout Not Supported: 0x%x",
			(u32)Func);
		return AGESA_UNSUPPORTED;
	}

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
	AGESA_STATUS Status;
	UINT8 Value;
	UINTN ResetType;
	AMD_CONFIG_PARAMS *StdHeader;

	ResetType = Data;
	StdHeader = ConfigPtr;

	/*
	 * This should perform the RESET based upon the ResetType, but coreboot
	 * doesn't have a reset manager to handle a WHENEVER case. Do all
	 * resets immediately.
	 */
	switch (ResetType) {
	case WARM_RESET_WHENEVER:
	case COLD_RESET_WHENEVER:
	case WARM_RESET_IMMEDIATELY:
	case COLD_RESET_IMMEDIATELY:
		Value = 0x06;
		LibAmdIoWrite(AccessWidth8, SYS_RESET, &Value, StdHeader);
		break;

	default:
		break;
	}

	Status = 0;
	return Status;
}

AGESA_STATUS agesa_RunFuncOnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;

	Status = agesawrapper_amdlaterunaptask(Func, Data, ConfigPtr);
	return Status;
}

AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData,
							VOID *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO *pVbiosImageInfo;

	pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_boot_map_with_leak(
			"pci"CONFIG_VGA_BIOS_ID".rom",
			CBFS_TYPE_OPTIONROM, NULL);
	printk(BIOS_DEBUG, "agesa_GfxGetVbiosImage: IMGptr=%p\n",
						pVbiosImageInfo->ImagePtr);
	return pVbiosImageInfo->ImagePtr ? AGESA_SUCCESS : AGESA_WARNING;
}

AGESA_STATUS agesa_RunFcnOnAllAps(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - Missing AGESA callout: %s\n", __func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}

AGESA_STATUS agesa_LateRunApTask(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - Missing AGESA callout: %s\n", __func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}


AGESA_STATUS agesa_PcieSlotResetControl(UINT32 Func, UINTN Data,
	VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - Missing AGESA callout: %s\n", __func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}

AGESA_STATUS agesa_WaitForAllApsFinished(UINT32 Func, UINTN Data,
	VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - Missing AGESA callout: %s\n", __func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}

AGESA_STATUS agesa_IdleAnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - Missing AGESA callout: %s\n", __func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}
