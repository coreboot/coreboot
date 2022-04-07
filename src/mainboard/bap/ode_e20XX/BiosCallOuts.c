/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <AGESA.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <FchPlatform.h>
#include <spd_bin.h>

#include "imc.h"

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,                 agesa_Reset },
	{AGESA_READ_SPD,                 board_ReadSpd_from_cbfs },
	{AGESA_READ_SPD_RECOVERY,        agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, agesa_NoopSuccess },
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  agesa_GfxGetVbiosImage }
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/**
 * ALC272 Verb Table
 */
const CODEC_ENTRY Alc272_VerbTbl[] = {
	{0x11, 0x411111F0}, //        - SPDIF_OUT2
	{0x12, 0x411111F0}, //        - DMIC_1/2
	{0x13, 0x411111F0}, //        - DMIC_3/4
	{0x14, 0x411111F0}, // Port D - LOUT1
	{0x15, 0x01011050}, // Port A - LOUT2 Explorer 2x DAC
	{0x16, 0x411111F0}, //
	{0x17, 0x411111F0}, // Port H - MONO
	{0x18, 0x01a11840}, // Port B - MIC1
	{0x19, 0x411111F0}, // Port F - MIC2
	{0x1a, 0x01811030}, // Port C - LINE1
	{0x1b, 0x01811020}, // Port E - LINE2 Explorer 2x ADC
	{0x1d, 0x40130605}, //        - PCBEEP
	{0x1e, 0x411111F0}, //        - SPDIF_OUT1
	{0x21, 0x01211010}, // Port I - HPOUT
	{0xff, 0xffffffff}
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

/* Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM failed to read the input temperature via I2C,
 *  if other software switches the I2C switch by mistake or intention.
 *  We recommend using IMC to control Fans, instead of HWM.
 */
static void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	/* Enable IMC fan control, the recommended way */
	if (CONFIG(HUDSON_IMC_FWM)) {
		imc_reg_init();

		/* HwMonitorEnable = TRUE &&  HwmFchtsiAutoOpll ==FALSE to call FchECfancontrolservice */
		FchParams->Hwm.HwMonitorEnable = TRUE;
		FchParams->Hwm.HwmFchtsiAutoPoll = FALSE;/* 0 disable, 1 enable TSI Auto Polling */

		FchParams->Imc.ImcEnable = TRUE;
		FchParams->Hwm.HwmControl = 1;	/* 1 IMC, 0 HWM */
		FchParams->Imc.ImcEnableOverWrite = 1; /* 2 disable IMC, 1 enable IMC, 0 following hw strap setting */

		LibAmdMemFill(&(FchParams->Imc.EcStruct), 0, sizeof(FCH_EC), FchParams->StdHeader);

		/* Thermal Zone Parameter */
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg0 = 0x00;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg1 = 0x00;	/* Zone */
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg2 = 0x3d; //BIT0 | BIT2 | BIT5;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg3 = 0x4e;//6 | BIT3;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg4 = 0x00;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg5 = 0x04;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg6 = 0x9a;	/* SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032 */
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg7 = 0x01;
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg8 = 0x01;	/* PWM stepping rate in unit of PWM level percentage */
		FchParams->Imc.EcStruct.MsgFun81Zone0MsgReg9 = 0x00;

		/* IMC Fan Policy temperature thresholds */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg0 = 0x00;
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg1 = 0x00;	/* Zone */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg2 =   50;	/*AC0 threshold in Celsius */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg3 =   45;	/*AC1 threshold in Celsius */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg4 =   40;	/*AC2 threshold in Celsius */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg5 = 0xff;	/*AC3 threshold in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg6 = 0xff;	/*AC4 threshold in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg7 = 0xff;	/*AC5 threshold in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg8 = 0xff;	/*AC6 threshold in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgReg9 = 0xff;	/*AC7 lowest threshold in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegA = 0x4b;	/*critical threshold* in Celsius, 0xFF is not define */
		FchParams->Imc.EcStruct.MsgFun83Zone0MsgRegB = 0x00;

		/* IMC Fan Policy PWM Settings */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg0 = 0x00;
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg1 = 0x00;	/* Zone */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg2 =  100;	/* AL0 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg3 =   99;	/* AL1 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg4 =   98;	/* AL2 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg5 = 0xff;	/* AL3 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg6 = 0xff;	/* AL4 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg7 = 0xff;	/* AL5 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg8 = 0xff;	/* AL6 percentage */
		FchParams->Imc.EcStruct.MsgFun85Zone0MsgReg9 = 0xff;	/* AL7 percentage */

		FchParams->Imc.EcStruct.IMCFUNSupportBitMap = 0x111;//BIT0 | BIT4 |BIT8;

		/* NOTE:
		 * FchInitLateHwm will overwrite the EcStruct with EcDefaultMessage,
		 * AGESA puts EcDefaultMessage as global data in ROM, so we can't overwrite it.
		 * So we remove it from AGESA code. Please See FchInitLateHwm.
		 */
	} else {
		/* HWM fan control, the way not recommended */
		FchParams->Imc.ImcEnable = FALSE;
		FchParams->Hwm.HwMonitorEnable = TRUE;
		FchParams->Hwm.HwmFchtsiAutoPoll = TRUE;/* 1 enable, 0 disable TSI Auto Polling */
	}
}

void board_FCH_InitReset(struct sysinfo *cb_NA, FCH_RESET_DATA_BLOCK *FchParams_reset)
{
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams_env)
{
	/* Azalia Controller OEM Codec Table Pointer */
	FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);

	/* Fan Control */
	oem_fan_control(FchParams_env);

	/* sata configuration */
	/* disable GEN2 limitation */
	FchParams_env->Sata.SataMode.SataSetMaxGen2 = FALSE;
}

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;
	u8 index;

	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	if (CONFIG(BAP_E20_DDR3_1066))
		index = 1;
	else	/* CONFIG_BAP_E20_DDR3_800 */
		index = 0;

	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_ddr3_spd_from_cbfs((u8 *)info->Buffer, index) < 0)
		die("No SPD data\n");

	return AGESA_SUCCESS;
}
