/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Micro Passion Ltd 
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
#include <arch/io.h>
#include <console/console.h>
#include <console/loglevel.h>
#include <string.h>

STATIC CONST BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,          fam16kb_AllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,        fam16kb_DeallocateBuffer },
	{AGESA_DO_RESET,                 fam16kb_Reset },
	{AGESA_LOCATE_BUFFER,            fam16kb_LocateBuffer },
	{AGESA_READ_SPD,						BiosReadSpd},
	{AGESA_READ_SPD_RECOVERY,        fam16kb_DefaultRet },
	{AGESA_RUNFUNC_ONAP,             fam16kb_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        fam16kb_GetIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  fam16kb_HookBeforeDQSTraining },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, fam16kb_HookBeforeExitSelfRefresh },
	{AGESA_FCH_OEM_CALLOUT,          Fch_Oem_config },
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  fam16kb_HookGfxGetVbiosImage }
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
 * AMD Kratos-X1 Pico ITX Platform ALC272 Verb Table
 */
static const CODEC_ENTRY kratosx1_Alc272_VerbTbl[] = {
	{0x11, 0x411111F0}, //        - SPDIF_OUT2
	{0x12, 0x411111F0}, //        - DMIC_1/2
	{0x13, 0x411111F0}, //        - DMIC_3/4
	{0x14, 0x411111F0}, // Port D - LOUT1
	{0x15, 0x411111F0}, // Port A - LOUT2
	{0x16, 0x411111F0}, //
	{0x17, 0x411111F0}, // Port H - MONO
	{0x18, 0x01a19840}, // Port B - MIC1
	{0x19, 0x411111F0}, // Port F - MIC2
	{0x1a, 0x01813030}, // Port C - LINE1
	{0x1b, 0x411111F0}, // Port E - LINE2
	{0x1d, 0x40130605}, //        - PCBEEP
	{0x1e, 0x411111F0},
	{0x21, 0x01211010},
	{0xff, 0xffffffff}
};

static const CODEC_TBL_LIST kratosx1CodecTableList[] =
{
	{0x10ec0272, (CODEC_ENTRY*)&kratosx1_Alc272_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

const unsigned char SPD_2GB_1R8_1333 [] = {
0x92,0x10,0x0B,0x03,0x03,0x19,0x00,0x01,0x03,0x52,0x01,0x08,0x0c,0x00,0x7e,0x00,
0x69,0x78,0x69,0x30,0x69,0x11,0x20,0x89,0x00,0x05,0x3C,0x3C,0x00,0xF0,0x83,0x81,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2F,0x11,0x61,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

AGESA_STATUS BiosReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
#ifdef __PRE_RAM__
	AGESA_READ_SPD_PARAMS * Params = (AGESA_READ_SPD_PARAMS *) ConfigPtr;

	printk(BIOS_DEBUG, "BiosReadSpd: %d %d %d\n", Params->SocketId, Params->MemChannelId, Params->DimmId);
	
	memcpy(Params->Buffer, SPD_2GB_1R8_1333, 256);

	Status = AGESA_SUCCESS;
#else
	Status = AGESA_UNSUPPORTED;
#endif

	return Status;
}

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such Azalia, SATA, IMC etc.
 */
AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr)
{
	FCH_RESET_DATA_BLOCK *FchParams = (FCH_RESET_DATA_BLOCK *)FchData;

	if (FchParams->StdHeader->Func == AMD_INIT_RESET) {
		//FCH_RESET_DATA_BLOCK *FchParams_reset =  (FCH_RESET_DATA_BLOCK *) FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		//FchParams_reset->EcChannel0 = TRUE; /* logical devicd 3 */
	} else if (FchParams->StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&kratosx1CodecTableList[0]);
		/* Azalia Controller Front Panel OEM Table Pointer */

		/* Fan Control */
		//oem_fan_control(FchParams_env);

		/* XHCI configuration */
#if CONFIG_HUDSON_XHCI_ENABLE
		FchParams_env->Usb.Xhci0Enable = TRUE;
#else
		FchParams_env->Usb.Xhci0Enable = FALSE;
#endif
		FchParams_env->Usb.Xhci1Enable = FALSE;

		/* sata configuration */
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}
