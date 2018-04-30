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
#include <compiler.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <timer.h>
#include <amdlib.h>
#include <amdblocks/BiosCallOuts.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <reset.h>
#include <soc/southbridge.h>

#if ENV_BOOTBLOCK
const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	{ AGESA_DO_RESET,                 agesa_Reset },
	{ AGESA_FCH_OEM_CALLOUT,          agesa_fch_initreset },
	{ AGESA_HALT_THIS_AP,             agesa_HaltThisAp },
	{ AGESA_HEAP_REBASE,              agesa_HeapRebase },
	{ AGESA_GNB_PCIE_SLOT_RESET,      agesa_PcieSlotResetControl }
};
#else
const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	/* Required callouts */
#if ENV_ROMSTAGE
	{ AGESA_HALT_THIS_AP,             agesa_HaltThisAp },
#endif
	{ AGESA_ALLOCATE_BUFFER,          agesa_AllocateBuffer },
	{ AGESA_DEALLOCATE_BUFFER,        agesa_DeallocateBuffer },
	{ AGESA_DO_RESET,                 agesa_Reset },
	{ AGESA_LOCATE_BUFFER,            agesa_LocateBuffer },
	{ AGESA_READ_SPD,                 agesa_ReadSpd },
	{ AGESA_GNB_PCIE_SLOT_RESET,      agesa_PcieSlotResetControl },
	{ AGESA_GET_TEMP_HEAP_BASE,       agesa_GetTempHeapBase },
	{ AGESA_HEAP_REBASE,              agesa_HeapRebase },
#if ENV_RAMSTAGE
	{ AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{ AGESA_RUNFUNC_ON_ALL_APS,       agesa_RunFcnOnAllAps },
	{ AGESA_WAIT_FOR_ALL_APS,         agesa_WaitForAllApsFinished },
	{ AGESA_IDLE_AN_AP,               agesa_IdleAnAp },
#endif /* ENV_RAMSTAGE */

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
		printk(BIOS_ERR, "ERROR: AGESA Callout Not Supported: 0x%x\n",
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
	case WARM_RESET_IMMEDIATELY:
		do_soft_reset();
		break;

	case COLD_RESET_WHENEVER:
	case COLD_RESET_IMMEDIATELY:
		do_hard_reset();
		break;

	default:
		break;
	}

	Status = 0;
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

AGESA_STATUS __weak platform_PcieSlotResetControl(UINT32 Func,
	UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s not supported\n",
		__func__);
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_PcieSlotResetControl(UINT32 Func, UINTN Data,
	VOID *ConfigPtr)
{
	return platform_PcieSlotResetControl(Func, Data, ConfigPtr);
}

/*
 * Application Processor callouts:
 * agesa_RunFuncOnAp() and agesa_RunFcnOnAllAps() are called after main memory
 * has been initialized and coreboot has taken control of AP task dispatching.
 * These functions execute callout_ap_entry() on each AP, which calls the
 * AmdLateRunApTask() entry point if it is a targeted AP.
 */

/*
 * Global data for APs.
 * Passed from the AGESA_Callout for the agesawrapper_amdlaterunaptask.
 */
static struct agesa_data {
	UINT32 Func;
	UINTN Data;
	VOID *ConfigPtr;
} agesadata;

/*
 * BSP deploys APs to callout_ap_entry(), which calls
 * agesawrapper_amdlaterunaptask with the agesadata.
 */
static void callout_ap_entry(void *unused)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	printk(BIOS_DEBUG, "%s Func: 0x%x,  Data: 0x%lx, Ptr: 0x%p \n",
		__func__, agesadata.Func, agesadata.Data, agesadata.ConfigPtr);

	/* Check if this AP should run the function */
	if (!((agesadata.Func == AGESA_RUNFUNC_ONAP) &&
	    (agesadata.Data == lapicid())))
		return;

	Status = agesawrapper_amdlaterunaptask(agesadata.Func, agesadata.Data,
			agesadata.ConfigPtr);

	if (Status)
		printk(BIOS_DEBUG, "There was a problem with %lx returned %s\n",
			lapicid(), decodeAGESA_STATUS(Status));
}

AGESA_STATUS agesa_RunFuncOnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_DEBUG, "%s\n", __func__);

	agesadata.Func = Func;
	agesadata.Data = Data;
	agesadata.ConfigPtr = ConfigPtr;
	mp_run_on_aps(callout_ap_entry, NULL, MP_RUN_ON_ALL_CPUS,
			100 * USECS_PER_MSEC);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_RunFcnOnAllAps(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_DEBUG, "%s\n", __func__);

	agesadata.Func = Func;
	agesadata.Data = Data;
	agesadata.ConfigPtr = ConfigPtr;
	mp_run_on_aps(callout_ap_entry, NULL, MP_RUN_ON_ALL_CPUS,
			100 * USECS_PER_MSEC);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_WaitForAllApsFinished(UINT32 Func, UINTN Data,
	VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s not supported\n",
		__func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}

AGESA_STATUS agesa_IdleAnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s no supported\n",
		__func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}
