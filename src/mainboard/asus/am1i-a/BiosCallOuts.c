/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Sergej Ivanov <getinaks@gmail.com>
 * Copyright (C) 2018 Gergely Kiss <mail.gery@gmail.com>
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
#include "AGESA.h"
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/state_machine.h>
#include "FchPlatform.h"
#include "cbfs.h"
#include <stdlib.h>
#include <pc80/mc146818rtc.h>
#include <types.h>

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
 * CODEC Initialization Table for Azalia HD Audio using Realtek ALC887-VD chip (from linux, running under vendor bios)
 */
const CODEC_ENTRY Alc887_VerbTbl[] =
{
	{ 0x11, 0x40330000 },
	{ 0x12, 0x411111f0 },
	{ 0x14, 0x01014010 },
	{ 0x15, 0x411111f0 },
	{ 0x16, 0x411111f0 },
	{ 0x17, 0x411111f0 },
	{ 0x18, 0x01a19030 },
	{ 0x19, 0x02a19040 },
	{ 0x1a, 0x0181303f },
	{ 0x1b, 0x02214020 },
	{ 0x1c, 0x411111f0 },
	{ 0x1d, 0x4024c601 },
	{ 0x1e, 0x411111f0 },
	{ 0x1f, 0x411111f0 }
};

static const CODEC_TBL_LIST CodecTableList[] =
{
	{0x10ec0887, (CODEC_ENTRY*)&Alc887_VerbTbl[0]},
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
	FchParams_reset->LegacyFree = IS_ENABLED(CONFIG_HUDSON_LEGACY_FREE);
	FchParams_reset->Mode = 6;

	/* Read SATA speed setting from CMOS */
	enum cb_err ret;
	ret = get_option(&FchParams_reset->SataSetMaxGen2, "sata_speed");
	if (ret != CB_SUCCESS) {
		FchParams_reset->SataSetMaxGen2 = 0;
		printk(BIOS_DEBUG, "ERROR: cannot read CMOS setting, falling back to default. Error code: %x\n", (int)ret);
	}
	printk(BIOS_DEBUG, "Force SATA 3Gbps mode = %x\n", FchParams_reset->SataSetMaxGen2);
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams_env)
{
	/* Azalia Controller OEM Codec Table Pointer */
	FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&CodecTableList[0]);

	/* Fan Control */
	FchParams_env->Imc.ImcEnable = FALSE;
	FchParams_env->Hwm.HwMonitorEnable = FALSE;
	FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */

	/* Read SATA controller mode from CMOS */
	enum cb_err ret;
	ret = get_option(&FchParams_env->Sata.SataClass, "sata_mode");
	if ( ret != CB_SUCCESS) {
		FchParams_env->Sata.SataClass = 0;
		printk(BIOS_DEBUG, "ERROR: cannot read CMOS setting, falling back to default. Error code: %x\n", (int)ret);
	}

	// code from olivehillplus (ft3b) - only place where sata is configured
	switch ((SATA_CLASS)FchParams_env->Sata.SataClass) {
	case SataLegacyIde:
	case SataRaid:
	case SataAhci:
	case SataAhci7804:
		FchParams_env->Sata.SataIdeMode = FALSE;
		printk(BIOS_DEBUG, "AHCI or RAID or IDE = %x\n", FchParams_env->Sata.SataClass);
		break;

	case SataIde2Ahci:
	case SataIde2Ahci7804:
	default: /* SataNativeIde */
		FchParams_env->Sata.SataIdeMode = TRUE;
		printk(BIOS_DEBUG, "IDE2AHCI = %x\n", FchParams_env->Sata.SataClass);
		break;
	}
}
