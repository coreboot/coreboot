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
 */

#include "AGESA.h"
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "OptionsIds.h"

#include <cbfs.h>
#include <vendorcode/amd/agesa/f15tn/Proc/Fch/FchPlatform.h>
#include <stdlib.h>

static AGESA_STATUS Fch_Oem_config(UINT32 Func, UINTN FchData, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,                 agesa_Reset },
	{AGESA_READ_SPD,                 agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,        agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, agesa_NoopSuccess },
	{AGESA_FCH_OEM_CALLOUT,          Fch_Oem_config },
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  agesa_GfxGetVbiosImage }
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/**
 * ASUS F2A85-M board ALC887-VD Verb Table
 *
 * Copied from `/sys/class/sound/hwC1D0/init_pin_configs` when running
 * the vendor BIOS.
 */
const CODEC_ENTRY f2a85_m_alc887_VerbTbl[] = {
	{0x11, 0x99430140},
	{0x12, 0x411111f0},
	{0x14, 0x01014010},
	{0x15, 0x01011012},
	{0x16, 0x01016011},
	{0x17, 0x01012014},
	{0x18, 0x01a19850},
	{0x19, 0x02a19c60},
	{0x1a, 0x0181305f},
	{0x1b, 0x02214c20},
	{0x1c, 0x411111f0},
	{0x1d, 0x4005e601},
	{0x1e, 0x01456130},
	{0x1f, 0x411111f0},
	{0xff, 0xffffffff}
};

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0887, (CODEC_ENTRY*)&f2a85_m_alc887_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such Azalia, SATA, GEC, IMC etc.
 */
static AGESA_STATUS Fch_Oem_config(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		FchParams_reset->LegacyFree = IS_ENABLED(CONFIG_HUDSON_LEGACY_FREE);
		FchParams_reset->FchReset.Xhci0Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
		FchParams_reset->FchReset.Xhci1Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
	} else if (StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);
		/* Azalia Controller Front Panel OEM Table Pointer */
		FchParams_env->Imc.ImcEnable = FALSE;
		FchParams_env->Hwm.HwMonitorEnable = FALSE;
		FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */

		/* XHCI configuration */
		FchParams_env->Usb.Xhci0Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
		FchParams_env->Usb.Xhci1Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}
