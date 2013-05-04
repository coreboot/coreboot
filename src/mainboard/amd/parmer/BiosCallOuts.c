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
 * AMD Parmer Platform ALC272 Verb Table
 */
static const CODEC_ENTRY Parmer_Alc272_VerbTbl[] = {
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

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0272, (CODEC_ENTRY*)&Parmer_Alc272_VerbTbl[0]},
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

/* Parmer Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM failed to read the input temperture vi I2C,
 *  if other software switch the I2C switch by mistake or intention.
 *  We recommend to using IMC to control Fans, instead of HWM.
 */
static void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	FCH_HWM_FAN_CTR oem_factl[5] = {
		/*temperatuer input, fan mode, frequency, low_duty, med_duty, multiplier, lowtemp, medtemp, hightemp, LinearRange, LinearHoldCount */
		/* Parmer FanOUT0 Fan header J32 */
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		/* Parmer FanOUT1 Fan header J31*/
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
		{FAN_INPUT_INTERNAL_DIODE, (FAN_STEPMODE | FAN_POLARITY_HIGH), FREQ_100HZ, 40, 60,  0, 40, 65, 85, 0, 0},
	};
	LibAmdMemCopy ((VOID *)(FchParams->Hwm.HwmFanControl), &oem_factl, (sizeof (FCH_HWM_FAN_CTR) * 5), FchParams->StdHeader);

	/* Enable IMC fan control. the recommand way */
#if defined CONFIG_HUDSON_IMC_FWM && (CONFIG_HUDSON_IMC_FWM == 1)
	/* HwMonitorEnable = TRUE &&  HwmFchtsiAutoOpll ==FALSE to call FchECfancontrolservice */
	FchParams->Hwm.HwMonitorEnable = TRUE;
	FchParams->Hwm.HwmFchtsiAutoPoll = FALSE;/* 0 disable, 1 enable TSI Auto Polling */

	FchParams->Imc.ImcEnable = TRUE;
	FchParams->Hwm.HwmControl = 1;	/* 1 IMC, 0 HWM */
	FchParams->Imc.ImcEnableOverWrite = 1; /* 2 disable IMC , 1 enable IMC, 0 following hw strap setting */

	LibAmdMemFill(&(FchParams->Imc.EcStruct), 0, sizeof(FCH_EC), FchParams->StdHeader);

	/* Thermal Zone Parameter */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg1 = 0x00;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg2 = 0x00; //BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg3 = 0x00;//6 | BIT3;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg6 = 0x98;	/* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg7 = 2;
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg8 = 0;	/* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg9 = 0;

	/* IMC Fan Policy temperature thresholds */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg1 = 0x00;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg2 = 0;///80;	/*AC0 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg3 = 0;	/*AC1 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg4 = 0;	/*AC2 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg5 = 0;	/*AC3 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg6 = 0;	/*AC4 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg7 = 0;	/*AC5 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg8 = 0;	/*AC6 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg9 = 0;	/*AC7 lowest threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegA = 0;	/*critical threshold* in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegB = 0x00;

	/* IMC Fan Policy PWM Settings */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg1 = 0x00;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg2 = 0;	/* AL0 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg3 = 0;	/* AL1 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg4 = 0;	/* AL2 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg5 = 0x00;	/* AL3 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg6 = 0x00;	/* AL4 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg7 = 0x00;	/* AL5 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg8 = 0x00;	/* AL6 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg9 = 0x00;	/* AL7 percentage */

	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg1 = 0x01;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg2 = 0x55;//BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg3 = 0x17;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg6 = 0x90;	/* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg7 = 0;
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg8 = 0;	/* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone1MsgReg9 = 0;

	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg1 = 0x01;	/* zone */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg2 = 60;	/*AC0 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg3 = 40;	/*AC1 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg4 = 0;	/*AC2 threshold in Celsius */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg5 = 0;	/*AC3 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg6 = 0;	/*AC4 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg7 = 0;	/*AC5 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg8 = 0;	/*AC6 threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgReg9 = 0;	/*AC7 lowest threshold in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgRegA = 0;	/*critical threshold* in Celsius, 0xFF is not define */
	FchParams->Imc.EcStruct.MsgFun83Zone1MsgRegB = 0x00;

	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg1 = 0x01;	/*Zone */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg2 = 0;	/* AL0 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg3 = 0;	/* AL1 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg4 = 0;	/* AL2 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg5 = 0x00;	/* AL3 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg6 = 0x00;	/* AL4 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg7 = 0x00;	/* AL5 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg8 = 0x00;	/* AL6 percentage */
	FchParams->Imc.EcStruct.MsgFun85Zone1MsgReg9 = 0x00;	/* AL7 percentage */

	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg1 = 0x2;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg2 = 0x0;//BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg3 = 0x0;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg6 = 0x98;	/* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg7 = 2;
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg8 = 5;	/* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone2MsgReg9 = 0;

	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg0 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg1 = 0x3;	/* Zone */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg2 = 0x0;//BIT0 | BIT2 | BIT5;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg3 = 0x0;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg4 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg5 = 0x00;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg6 = 0x0;	/* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg7 = 0;
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg8 = 0;	/* PWM steping rate in unit of PWM level percentage */
	FchParams->Imc.EcStruct.MsgFun81Zone3MsgReg9 = 0;

	/* IMC Function */
	FchParams->Imc.EcStruct.IMCFUNSupportBitMap = 0x333;//BIT0 | BIT4 |BIT8;

	/* NOTE:
	 * FchInitLateHwm will overwrite the EcStruct with EcDefaultMassege,
	 * AGESA put EcDefaultMassege as global data in ROM, so we can't overwride it.
	 * so we remove it from AGESA code. Please Seee FchInitLateHwm.
	 */

#else /* HWM fan control, the way not recommand */
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
AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr)
{
	FCH_RESET_DATA_BLOCK *FchParams = (FCH_RESET_DATA_BLOCK *)FchData;

	if (FchParams->StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset =  (FCH_RESET_DATA_BLOCK *) FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		//FchParams_reset->EcChannel0 = TRUE; /* logical devicd 3 */
#if CONFIG_HUDSON_LEGACY_FREE
		FchParams_reset->LegacyFree = 1;
#endif
	} else if (FchParams->StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);
		/* Azalia Controller Front Panel OEM Table Pointer */

		/* Fan Control */
		oem_fan_control(FchParams_env);

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
