/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2014 Advanced Micro Devices, Inc.
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
#include "amdlib.h"
#include <northbridge/amd/pi/BiosCallOuts.h>
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "FchPlatform.h"
#include "cbfs.h"
#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)
#include "imc.h"
#endif
#include "hudson.h"
#include <stdlib.h>
#include <device/azalia.h>

static AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,          agesa_AllocateBuffer },
	{AGESA_DEALLOCATE_BUFFER,        agesa_DeallocateBuffer },
	{AGESA_LOCATE_BUFFER,            agesa_LocateBuffer },
	{AGESA_READ_SPD,                 agesa_ReadSpd },
	{AGESA_DO_RESET,                 agesa_Reset },
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
 * Realtek ALC272 CODEC Verb Table
 */
static const CODEC_ENTRY Alc272_VerbTbl[] = {
	{ 0x11, 0x411111F0 },               /*        - S/PDIF Output 2         */
	{ 0x12, 0x411111F0 },               /*        - Digital Mic 1/2 [GPIO0] */
	{ 0x13, 0x411111F0 },               /*        - Digital Mic 3/4 [GPIO1] */
	{ 0x14, 0x411111F0 },               /* Port D - Front Panel headphone   */
	{ 0x15, 0x411111F0 },               /* Port A - Surround                */
	{ 0x17, 0x411111F0 },               /* Port H - Mono                    */
	{ 0x18,                             /* Port B - MIC - pink jack         */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_MICROPHONE << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_PINK << 12)
			| (4 << 4)
			| (0 << 0)
	},
	{ 0x19, 0x411111F0 },               /* Port F - Front Panel Mic        */
	{ 0x1A,                             /* Port C - LineIn1 - blue jack    */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_LINEIN << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_BLUE << 12)
			| (4 << 4)
			| (0xF << 0)
	},
	{ 0x1B, 0x411111F0 },               /* Port E - Front Panel line-out   */
	{ 0x1D, 0x40251E05 },               /* PC Beep - (internal)            */
	{ 0x1E,                             /* S/PDIF - Internal Header        */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_FRONT) << 24)
			| (AZALIA_PINCFG_DEVICE_SPDIF_OUT << 20)
			| (AZALIA_PINCFG_CONN_RCA << 16)
			| (AZALIA_PINCFG_COLOR_BLACK << 12)
			| (1 << 4)
			| (0 << 0)
	},
	{ 0x21,                             /* Port I - HPout - green jack     */
			(AZALIA_PINCFG_PORT_JACK << 30)
			| ((AZALIA_PINCFG_LOCATION_EXTERNAL | AZALIA_PINCFG_LOCATION_REAR) << 24)
			| (AZALIA_PINCFG_DEVICE_HP_OUT << 20)
			| (AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK << 16)
			| (AZALIA_PINCFG_COLOR_GREEN << 12)
			| (4 << 4)
			| (0 << 0)
	},
	{ 0xFF, 0xFFFFFFFF },
};

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0272, (CODEC_ENTRY*)&Alc272_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

#define FAN_INPUT_INTERNAL_DIODE	0
#define FAN_INPUT_TEMP0			1
#define FAN_INPUT_TEMP1			2
#define FAN_INPUT_TEMP2			3
#define FAN_INPUT_TEMP3			4
#define FAN_INPUT_TEMP0_FILTER		5
#define FAN_INPUT_ZERO			6
#define FAN_INPUT_DISABLED		7

#define FAN_AUTOMODE			(1 << 0)
#define FAN_LINEARMODE			(1 << 1)
#define FAN_STEPMODE			~(1 << 1)
#define FAN_POLARITY_HIGH		(1 << 2)
#define FAN_POLARITY_LOW		~(1 << 2)

/* Normally, 4-wire fan runs at 25KHz and 3-wire fan runs at 100Hz */
#define FREQ_28KHZ			0x0
#define FREQ_25KHZ			0x1
#define FREQ_23KHZ			0x2
#define FREQ_21KHZ			0x3
#define FREQ_29KHZ			0x4
#define FREQ_18KHZ			0x5
#define FREQ_100HZ			0xF7
#define FREQ_87HZ			0xF8
#define FREQ_58HZ			0xF9
#define FREQ_44HZ			0xFA
#define FREQ_35HZ			0xFB
#define FREQ_29HZ			0xFC
#define FREQ_22HZ			0xFD
#define FREQ_14HZ			0xFE
#define FREQ_11HZ			0xFF

/*
 * Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM will fail to read the input temperature via I2C if other
 *  software switches the I2C address.  AMD recommends using IMC
 *  to control fans, instead of HWM.
 */
static void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	FCH_HWM_FAN_CTR oem_factl[5] = {
		/*temperature input, fan mode, frequency, low_duty, med_duty, multiplier, lowtemp, medtemp, hightemp, LinearRange, LinearHoldCount */
		/* FanOUT0 Fan header J32 */
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		/* FanOUT1 Fan header J31*/
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
	};
	LibAmdMemCopy ((VOID *)(FchParams->Hwm.HwmFanControl), &oem_factl, (sizeof (FCH_HWM_FAN_CTR) * 5), FchParams->StdHeader);

	/* Enable IMC fan control. the recommended way */
#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)

	imc_reg_init();

	/* HwMonitorEnable = TRUE &&  HwmFchtsiAutoOpll ==FALSE to call FchECfancontrolservice */
	FchParams->Hwm.HwMonitorEnable = TRUE;
	FchParams->Hwm.HwmFchtsiAutoPoll = FALSE;       /* 0 disable, 1 enable TSI Auto Polling */

	FchParams->Imc.ImcEnable = TRUE;
	FchParams->Hwm.HwmControl = 1;	/* 1 IMC, 0 HWM */
	FchParams->Imc.ImcEnableOverWrite = 1; /* 2 disable IMC , 1 enable IMC, 0 following hw strap setting */

	LibAmdMemFill(&(FchParams->Imc.EcStruct), 0, sizeof(FCH_EC), FchParams->StdHeader);

	/* Thermal Zone Parameter */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg1 = 0x00;    /* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg2 = 0x3d;    //BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg3 = 0x0e;    //6 | BIT3;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg5 = 0x54;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg6 = 0x98;    /* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg7 = 0x02;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg8 = 0x01;    /* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg9 = 0x00;

	/* IMC Fan Policy temperature thresholds */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg1 = 0x00;    /* Zone */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg2 = 0x46;    /*AC0 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg3 = 0x3c;    /*AC1 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg4 = 0x32;    /*AC2 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg5 = 0xff;    /*AC3 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg6 = 0xff;    /*AC4 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg7 = 0xff;    /*AC5 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg8 = 0xff;    /*AC6 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg9 = 0xff;    /*AC7 lowest threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegA = 0x4b;    /*critical threshold* in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegB = 0x00;

	/* IMC Fan Policy PWM Settings */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg1 = 0x00;    /* Zone */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg2 = 0x5a;    /* AL0 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg3 = 0x46;    /* AL1 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg4 = 0x28;    /* AL2 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg5 = 0xff;    /* AL3 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg6 = 0xff;    /* AL4 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg7 = 0xff;    /* AL5 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg8 = 0xff;    /* AL6 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg9 = 0xff;    /* AL7 percentage */

	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg1 = 0x01;    /* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg2 = 0x55;    //BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg3 = 0x17;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg6 = 0x90;    /* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg7 = 0;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg8 = 0;       /* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg9 = 0;

	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg1 = 0x01;    /* zone */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg2 = 60;      /*AC0 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg3 = 40;      /*AC1 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg4 = 0;       /*AC2 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg5 = 0;       /*AC3 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg6 = 0;       /*AC4 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg7 = 0;       /*AC5 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg8 = 0;       /*AC6 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg9 = 0;       /*AC7 lowest threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgRegA = 0;       /*critical threshold* in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgRegB = 0x00;

	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg1 = 0x01;    /*Zone */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg2 = 0;       /* AL0 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg3 = 0;       /* AL1 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg4 = 0;       /* AL2 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg5 = 0x00;    /* AL3 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg6 = 0x00;    /* AL4 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg7 = 0x00;    /* AL5 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg8 = 0x00;    /* AL6 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg9 = 0x00;    /* AL7 percentage */

	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg1 = 0x2;     /* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg2 = 0x0;     //BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg3 = 0x0;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg6 = 0x98;    /* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg7 = 2;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg8 = 5;       /* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg9 = 0;

	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg1 = 0x3;     /* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg2 = 0x0;     //BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg3 = 0x0;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg6 = 0x0;     /* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg7 = 0;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg8 = 0;       /* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg9 = 0;

	/* IMC Function */
	FchParams->Imc.EcStruct.IMCFUNSupportBitMap = 0x333;//BIT0 | BIT4 |BIT8;

	/* NOTE:
	 * FchInitLateHwm will overwrite the EcStruct with EcDefaultMessage,
	 * AGESA put EcDefaultMessage as global data in ROM, so we can't override it.
	 * so we remove it from AGESA code. Please See FchInitLateHwm.
	 */

#else /* HWM fan control, using the alternative method */
	FchParams->Imc.ImcEnable = FALSE;
	FchParams->Hwm.HwMonitorEnable = TRUE;
	FchParams->Hwm.HwmFchtsiAutoPoll = TRUE;/* 1 enable, 0 disable TSI Auto Polling */

#endif /* CONFIG_HUDSON_IMC_FWM */
}

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such Azalia, SATA, IMC etc.
 */
static AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = (AMD_CONFIG_PARAMS *)ConfigPtr;
	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams =  (FCH_RESET_DATA_BLOCK *) FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		//FchParams->EcChannel0 = TRUE; /* logical devicd 3 */
		FchParams->LegacyFree = IS_ENABLED(CONFIG_HUDSON_LEGACY_FREE);

		/* Turn on FCH GPP slots */
		FchParams->FchReset.GppEnable = TRUE;
		FchParams->Gpp.GppFunctionEnable = TRUE;
		FchParams->Gpp.GppLinkConfig = PortA1B1C1D1;
		FchParams->Gpp.PortCfg[0].PortPresent = TRUE;
		FchParams->Gpp.PortCfg[1].PortPresent = TRUE;
		FchParams->Gpp.PortCfg[2].PortPresent = TRUE;
		FchParams->Gpp.PortCfg[3].PortPresent = TRUE;
		FchParams->FchReset.SataEnable = (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 2);
		FchParams->FchReset.IdeEnable = (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);

	} else if (StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Turn on FCH GPP slots */
		FchParams->Gpp.GppFunctionEnable = TRUE;
		FchParams->Gpp.GppLinkConfig = PortA1B1C1D1;

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams->Azalia.AzaliaPinCfg = TRUE;
		FchParams->Azalia.AzaliaConfig = (const AZALIA_PIN){
			.AzaliaSdin0 = (CONFIG_AZ_PIN>>0) & 0x03,
			.AzaliaSdin1 = (CONFIG_AZ_PIN>>2) & 0x03,
			.AzaliaSdin2 = (CONFIG_AZ_PIN>>4) & 0x03,
			.AzaliaSdin3 = (CONFIG_AZ_PIN>>6) & 0x03
		};
		FchParams->Azalia.AzaliaOemCodecTablePtr = CodecTableList;
		/* Azalia Controller Front Panel OEM Table Pointer */

		/* Fan Control */
		oem_fan_control(FchParams);
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}
