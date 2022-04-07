/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <timer.h>
#include <amdblocks/BiosCallOuts.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <amdblocks/reset.h>
#include <soc/southbridge.h>
#include <types.h>

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
#if ENV_RAMINIT
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

AGESA_STATUS GetBiosCallout(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	uintptr_t i;

	for (i = 0 ; i < BiosCalloutsLen ; i++) {
		if (BiosCallouts[i].CalloutName == Func)
			break;
	}

	if (i >= BiosCalloutsLen) {
		printk(BIOS_ERR, "AGESA Callout Not Supported: 0x%x\n",
			(u32)Func);
		return AGESA_UNSUPPORTED;
	}

	return BiosCallouts[i].CalloutPtr(Func, Data, ConfigPtr);
}

AGESA_STATUS agesa_NoopUnsupported(uint32_t Func, uintptr_t Data,
							void *ConfigPtr)
{
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_NoopSuccess(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_EmptyIdsInitData(uint32_t Func, uintptr_t Data,
							void *ConfigPtr)
{
	IDS_NV_ITEM *IdsPtr = ((IDS_CALLOUT_STRUCT *) ConfigPtr)->IdsNvPtr;
	if (Data == IDS_CALLOUT_INIT)
		IdsPtr[0].IdsNvValue = IdsPtr[0].IdsNvId = 0xffff;
	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_Reset(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	AGESA_STATUS Status;
	uintptr_t ResetType;

	ResetType = Data;

	/*
	 * This should perform the RESET based upon the ResetType, but coreboot
	 * doesn't have a reset manager to handle a WHENEVER case. Do all
	 * resets immediately.
	 */
	switch (ResetType) {
	case WARM_RESET_WHENEVER:
	case WARM_RESET_IMMEDIATELY:
		warm_reset();
		break;

	case COLD_RESET_WHENEVER:
	case COLD_RESET_IMMEDIATELY:
		cold_reset();
		break;

	default:
		break;
	}

	Status = 0;
	return Status;
}

AGESA_STATUS agesa_GfxGetVbiosImage(uint32_t Func, uintptr_t FchData,
							void *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO *pVbiosImageInfo;

	pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_map(
			"pci"CONFIG_VGA_BIOS_ID".rom", NULL);
	printk(BIOS_DEBUG, "%s: IMGptr=%p\n", __func__,
			pVbiosImageInfo->ImagePtr);
	return pVbiosImageInfo->ImagePtr ? AGESA_SUCCESS : AGESA_WARNING;
}

AGESA_STATUS __weak platform_PcieSlotResetControl(uint32_t Func,
	uintptr_t Data, void *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s not supported\n",
		__func__);
	return AGESA_UNSUPPORTED;
}

AGESA_STATUS agesa_PcieSlotResetControl(uint32_t Func, uintptr_t Data,
	void *ConfigPtr)
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
	uint32_t Func;
	uintptr_t Data;
	AP_EXE_PARAMS *ConfigPtr;
} agesadata;

/*
 * BSP deploys APs to callout_ap_entry(), which calls
 * agesawrapper_amdlaterunaptask with the agesadata.
 */
static void callout_ap_entry(void *unused)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	printk(BIOS_DEBUG, "%s Func: 0x%x,  Data: 0x%lx, Ptr: %p\n",
		__func__, agesadata.Func, agesadata.Data, agesadata.ConfigPtr);

	/* Check if this AP should run the function */
	if (!((agesadata.Func == AGESA_RUNFUNC_ONAP) &&
	    (agesadata.Data == lapicid())))
		return;

	Status = amd_late_run_ap_task(agesadata.ConfigPtr);

	if (Status)
		printk(BIOS_DEBUG, "There was a problem with %x returned %s\n",
			lapicid(), decodeAGESA_STATUS(Status));
}

AGESA_STATUS agesa_RunFuncOnAp(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	printk(BIOS_DEBUG, "%s\n", __func__);

	agesadata.Func = Func;
	agesadata.Data = Data;
	agesadata.ConfigPtr = ConfigPtr;
	if (mp_run_on_aps(callout_ap_entry, NULL, MP_RUN_ON_ALL_CPUS, 100 * USECS_PER_MSEC) !=
			CB_SUCCESS)
		return AGESA_ERROR;

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_RunFcnOnAllAps(uint32_t Func, uintptr_t Data,
							void *ConfigPtr)
{
	printk(BIOS_DEBUG, "%s\n", __func__);

	agesadata.Func = Func;
	agesadata.Data = Data;
	agesadata.ConfigPtr = ConfigPtr;
	if (mp_run_on_aps(callout_ap_entry, NULL, MP_RUN_ON_ALL_CPUS, 100 * USECS_PER_MSEC) !=
			CB_SUCCESS)
		return AGESA_ERROR;

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_WaitForAllApsFinished(uint32_t Func, uintptr_t Data,
	void *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s not supported\n",
		__func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}

AGESA_STATUS agesa_IdleAnAp(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	printk(BIOS_WARNING, "Warning - AGESA callout: %s no supported\n",
		__func__);
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	return Status;
}
