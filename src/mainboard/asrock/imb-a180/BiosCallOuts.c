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

#include <device/azalia.h>
#include <AGESA.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <FchPlatform.h>

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,                 agesa_Reset },
	{AGESA_READ_SPD,                 agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,        agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, agesa_NoopSuccess },
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  agesa_GfxGetVbiosImage }
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/**
 * CODEC Initialization Table for Azalia HD Audio using Realtek ALC662 chip
 */
static const CODEC_ENTRY Alc662_VerbTbl[] =
{
	{ 0x14, /*01014010*/                /* Port D - green headphone jack    */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_LINEOUT << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_GREEN << 12)
			| (1 << 4)
			| (0 << 0)
	},
	{ 0x15, /*0x90170120*/              /* Port A - white speaker header    */
			(AZALIA_PINCFG_PORT_FIXED << 30)
			| (AZALIA_PINCFG_LOCATION_INTERNAL << 24)
			| (AZALIA_PINCFG_DEVICE_SPEAKER << 20)
			| (AZALIA_PINCFG_CONN_OTHER_ANALOG << 16)
			| (AZALIA_PINCFG_COLOR_WHITE << 12)
			| (AZALIA_PINCFG_MISC_IGNORE_PRESENCE << 8)
			| (2 << 4)
			| (0 << 0)
	},
	{ 0x16, 0x411111F0 },               /* Port G - not connected           */
	{ 0x18, /*0x01A19040*/              /* Port B - pink headphone jack     */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_MICROPHONE << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_PINK << 12)
			| (4 << 4)
			| (0 << 0)
	},
	{ 0x19, /*0x02A19050*/              /* Port F - front panel header mic  */
			(AZALIA_PINCFG_PORT_NC << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_FRONT) << 24)
			| (AZALIA_PINCFG_DEVICE_MICROPHONE << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_PINK << 12)
			| (5 << 4)
			| (0 << 0)
	},
	{ 0x1A, /*0x0181304F*/              /* Port C - NL blue headphone jack  */
			(AZALIA_PINCFG_PORT_NC << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_LINEIN << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_BLUE << 12)
			| (4 << 4)
			| (0xF << 0)
	},
	{ 0x1B, /*0x02214030*/              /* Port E - front panel line-out     */
			(AZALIA_PINCFG_PORT_NC << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_FRONT) << 24)
			| (AZALIA_PINCFG_DEVICE_HP_OUT << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_GREEN << 12)
			| (3 << 4)
			| (0 << 0)
	},
	{ 0x1C, 0x411111F0 },               /* CD-in - Not Connected            */
	{ 0x1D, 0x411111F0 },               /* PC Beep - Not Connected          */
	{ 0x1E, 0x411111F0 },               /* S/PDIF - Not connected           */
	{ 0xFF, 0xFFFFFFFF },
};

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0662, (CODEC_ENTRY*)Alc662_VerbTbl},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

void board_FCH_InitReset(struct sysinfo *cb_NA, FCH_RESET_DATA_BLOCK *FchParams_reset)
{
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams_env)
{
	/* Azalia Controller OEM Codec Table Pointer */
	FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST*)CodecTableList;
}
