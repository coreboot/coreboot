/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "FchPlatform.h"
#include "cbfs.h"

STATIC CONST BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,          fam15tn_AllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,        fam15tn_DeallocateBuffer },
	{AGESA_DO_RESET,                 fam15tn_Reset },
	{AGESA_LOCATE_BUFFER,            fam15tn_LocateBuffer },
	{AGESA_READ_SPD,                 fam15tn_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,        fam15tn_DefaultRet },
	{AGESA_RUNFUNC_ONAP,             fam15tn_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        fam15tn_GetIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  fam15tn_HookBeforeDQSTraining },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, fam15tn_HookBeforeExitSelfRefresh },
	{AGESA_FCH_OEM_CALLOUT,          Fch_Oem_config },
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  fam15tn_HookGfxGetVbiosImage }
};

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN i;
	AGESA_STATUS CalloutStatus;
	UINTN CallOutCount = sizeof (BiosCallouts) / sizeof (BiosCallouts [0]);

	for (i = 0; i < CallOutCount; i++)
	{
		if (BiosCallouts[i].CalloutName == Func)
			break;
	}

	if(i >= CallOutCount)
		return AGESA_UNSUPPORTED;

	CalloutStatus = BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);

	return CalloutStatus;
}

/**
 * AMD Thatcher Platform ALC272 Verb Table
 */
static const CODEC_ENTRY Thatcher_Alc272_VerbTbl[] = {
	{0x11, 0x411111F0},
	{0x12, 0x411111F0},
	{0x13, 0x411111F0},
	{0x14, 0x411111F0},
	{0x15, 0x411111F0},
	{0x16, 0x411111F0},
	{0x17, 0x411111F0},
	{0x18, 0x01a19840},
	{0x19, 0x411111F0},
	{0x1a, 0x01813030},
	{0x1b, 0x411111F0},
	{0x1d, 0x40130605},
	{0x1e, 0x01441120},
	{0x21, 0x01211010},
	{0xff, 0xffffffff}
};

static const CODEC_TBL_LIST ThatcherCodecTableList[] =
{
	{0x10ec0272, (CODEC_ENTRY*)&Thatcher_Alc272_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such Azalia, SATA, GEC, IMC etc.
 */
AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr)
{
	FCH_RESET_DATA_BLOCK *FchParams = (FCH_RESET_DATA_BLOCK *)FchData;

	if (FchParams->StdHeader->Func == AMD_INIT_RESET) {
#if CONFIG_HUDSON_LEGACY_FREE
		FCH_RESET_DATA_BLOCK *FchParams_reset =  (FCH_RESET_DATA_BLOCK *) FchData;
		FchParams_reset->LegacyFree = 1;
#endif
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
	} else if (FchParams->StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&ThatcherCodecTableList[0]);
		/* Azalia Controller Front Panel OEM Table Pointer */
		FchParams_env->Imc.ImcEnable = FALSE;
		FchParams_env->Hwm.HwMonitorEnable = FALSE;
		FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */

		/* XHCI configuration */
#if CONFIG_HUDSON_XHCI_ENABLE
		FchParams_env->Usb.Xhci0Enable = TRUE;
		FchParams_env->Usb.Xhci1Enable = TRUE;
#else
		FchParams_env->Usb.Xhci0Enable = FALSE;
		FchParams_env->Usb.Xhci1Enable = FALSE;
#endif
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}
