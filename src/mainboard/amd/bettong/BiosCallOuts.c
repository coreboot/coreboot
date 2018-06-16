/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
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

#include <device/pci_def.h>
#include <device/device.h>
#include "AGESA.h"
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/pi/00660F01/chip.h>
#include "FchPlatform.h"
#include "cbfs.h"
#include "imc.h"
#include "hudson.h"
#include <stdlib.h>
#include <string.h>
#include <northbridge/amd/pi/dimmSpd.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <boardid.h>

static AGESA_STATUS Fch_Oem_config(UINT32 Func, UINTN FchData, VOID *ConfigPtr);
static AGESA_STATUS board_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_READ_SPD,                 board_ReadSpd },
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

static const GPIO_CONTROL oem_bettong_gpio[] = {
	{86, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE | FCH_GPIO_OUTPUT_ENABLE | DrvStrengthSel_12mA},
	{64, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE | FCH_GPIO_OUTPUT_ENABLE | DrvStrengthSel_12mA},
	{-1}
};

/* Bettong Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM will fail to read the input temperature via I2C if other
 *  software switches the I2C address.  AMD recommends using IMC
 *  to control fans, instead of HWM.
 */
static void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	/* Enable IMC fan control. the recommand way */
	imc_reg_init();

	FchParams->Imc.ImcEnable = TRUE;
	FchParams->Hwm.HwmControl = 1; /* 1 IMC, 0 HWM */
	FchParams->Imc.ImcEnableOverWrite = 1; /* 2 disable IMC, 1 enable IMC, 0 following hw strap setting */

	memset(&FchParams->Imc.EcStruct, 0, sizeof(FCH_EC));
}

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such as Azalia, SATA, IMC etc.
 */
AGESA_STATUS Fch_Oem_config(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");

		FchParams_reset->FchReset.Xhci0Enable = IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE);
		FchParams_reset->FchReset.Xhci1Enable = FALSE;
		FchParams_reset->EarlyOemGpioTable = oem_bettong_gpio;
	} else if (StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		if (IS_ENABLED(CONFIG_HUDSON_IMC_FWM))
			oem_fan_control(FchParams_env);

		/* XHCI configuration */
		if (IS_ENABLED(CONFIG_HUDSON_XHCI_ENABLE))
			FchParams_env->Usb.Xhci0Enable = TRUE;
		else
			FchParams_env->Usb.Xhci0Enable = FALSE;

		FchParams_env->Usb.Xhci1Enable = FALSE;
		FchParams_env->Usb.USB30PortInit = 8; /* 8: If USB3 port is unremoveable. */

		/* sata configuration */
		/* SD configuration */
		/* Rev F has an on-board eMMC, which only supports SD 2.0 */
		if (board_id() == 'F') {
			FchParams_env->Sd.SdConfig = SdVer2;
		}
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}

static AGESA_STATUS board_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
#ifdef __PRE_RAM__
	int spdAddress;
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;

	DEVTREE_CONST struct device *dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	DEVTREE_CONST struct northbridge_amd_pi_00660F01_config *config = dev->chip_info;
	UINT8 spdAddrLookup_rev_F [2][2][4]= {
		{ {0xA0, 0xA2}, {0xA4, 0xAC}, }, /* socket 0 - Channel 0 & 1 - 8-bit SPD addresses */
		{ {0x00, 0x00}, {0x00, 0x00}, }, /* socket 1 - Channel 0 & 1 - 8-bit SPD addresses */
	};

	if ((dev == 0) || (config == 0))
		return AGESA_ERROR;
	if (info->SocketId >= ARRAY_SIZE(config->spdAddrLookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(config->spdAddrLookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(config->spdAddrLookup[0][0]))
		return AGESA_ERROR;
	if (board_id() == 'F')
		spdAddress = spdAddrLookup_rev_F
			[info->SocketId] [info->MemChannelId] [info->DimmId];
	else
		spdAddress = config->spdAddrLookup
			[info->SocketId] [info->MemChannelId] [info->DimmId];

	if (spdAddress == 0)
		return AGESA_ERROR;
	int err = hudson_readSpd(spdAddress, (void *) info->Buffer, 128);
	if (err)
		return AGESA_ERROR;
#endif
	return AGESA_SUCCESS;
}
