/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia.h>
#include <AGESA.h>
#include <console/console.h>
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
 * CODEC Initialization Table for Azalia HD Audio using Realtek ALC662 chip (from linux, running under vendor bios)
 */
const CODEC_ENTRY Alc662_VerbTbl[] =
{
  { 0x14, 0x01014410 },
  { 0x15, 0x411111f0 },
  { 0x16, 0x411111f0 },
  { 0x18, 0x01a19c30 },
  { 0x19, 0x02a19c40 },
  { 0x1a, 0x0181343f },
  { 0x1b, 0x02214c20 },
  { 0x1c, 0x411111f0 },
  { 0x1d, 0x4004c601 },
  { 0x1e, 0x411111f0 },
  { 0xff, 0xffffffff }
};

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0662, (CODEC_ENTRY*)&Alc662_VerbTbl[0]},
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

void board_FCH_InitReset(struct sysinfo *cb_NA, FCH_RESET_DATA_BLOCK *FchParams_reset)
{
	FchParams_reset->LegacyFree = CONFIG(HUDSON_LEGACY_FREE);
	FchParams_reset->Mode = 6;
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams_env)
{
	/* Azalia Controller OEM Codec Table Pointer */
	FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);

	/* Fan Control */
	FchParams_env->Imc.ImcEnable = FALSE;
	FchParams_env->Hwm.HwMonitorEnable = FALSE;
	FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */

	FchParams_env->Sata.SataClass = CONFIG_HUDSON_SATA_MODE;

	switch ((SATA_CLASS)CONFIG_HUDSON_SATA_MODE) {
	case SataLegacyIde:
	case SataRaid:
	case SataAhci:
	case SataAhci7804:
		FchParams_env->Sata.SataIdeMode = FALSE;
		printk(BIOS_DEBUG, "AHCI or RAID or IDE = %x\n", CONFIG_HUDSON_SATA_MODE);
		break;

	case SataIde2Ahci:
	case SataIde2Ahci7804:
	default: /* SataNativeIde */
		FchParams_env->Sata.SataIdeMode = TRUE;
		printk(BIOS_DEBUG, "IDE2AHCI = %x\n", CONFIG_HUDSON_SATA_MODE);
		break;
	}
}
