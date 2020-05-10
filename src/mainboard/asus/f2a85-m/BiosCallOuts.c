/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/state_machine.h>

#include <vendorcode/amd/agesa/f15tn/Proc/Fch/FchPlatform.h>

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
 * ASUS F2A85-M board ALC887-VD Verb Table
 *
 * Copied from `/sys/class/sound/hwC1D0/init_pin_configs` when running
 * the vendor BIOS.
 */
#if !CONFIG(BOARD_ASUS_F2A85_M_LE)
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
#else
const CODEC_ENTRY f2a85_m_alc887_VerbTbl[] = {
	{0x11, 0x99430140},
	{0x12, 0x411111f0},
	{0x14, 0x01014010},
	{0x15, 0x411111f0},
	{0x16, 0x411111f0},
	{0x17, 0x411111f0},
	{0x18, 0x01a19850},
	{0x19, 0x02a19c60},
	{0x1a, 0x0181305f},
	{0x1b, 0x02214c20},
	{0x1c, 0x411111f0},
	{0x1d, 0x4004c601},
	{0x1e, 0x01456130},
	{0x1f, 0x411111f0},
	{0xff, 0xffffffff}
};
#endif

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0887, (CODEC_ENTRY*)&f2a85_m_alc887_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

void board_FCH_InitReset(struct sysinfo *cb_NA, FCH_RESET_DATA_BLOCK *FchParams_reset)
{
	FchParams_reset->LegacyFree = CONFIG(HUDSON_LEGACY_FREE);
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams_env)
{
	/* Azalia Controller OEM Codec Table Pointer */
	FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);

	/* Fan Control */
	FchParams_env->Imc.ImcEnable = FALSE;
	FchParams_env->Hwm.HwMonitorEnable = FALSE;
	FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */
}
