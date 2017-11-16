/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 * Copyright (C) 2017 Google Inc.
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
#include <BiosCallOuts.h>
#include <soc/southbridge.h>

#include <agesawrapper.h>
#include <amdlib.h>
#include <amdblocks/dimm_spd.h>

AGESA_STATUS agesa_fch_initreset(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset;
		FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		FchParams_reset->FchReset.SataEnable = sb_sata_enable();
		FchParams_reset->FchReset.IdeEnable = sb_ide_enable();

		/* Get platform specific configuration changes */
		platform_FchParams_reset(FchParams_reset);

		printk(BIOS_DEBUG, "Done\n");
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_fch_initenv(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		if (IS_ENABLED(CONFIG_STONEYRIDGE_IMC_FWM))
			oem_fan_control(FchParams_env);

		/* XHCI configuration */
		if (IS_ENABLED(CONFIG_STONEYRIDGE_XHCI_ENABLE))
			FchParams_env->Usb.Xhci0Enable = TRUE;
		else
			FchParams_env->Usb.Xhci0Enable = FALSE;
		FchParams_env->Usb.Xhci1Enable = FALSE;

		/* 8: If USB3 port is unremoveable. */
		FchParams_env->Usb.USB30PortInit = 8;

		/* SATA configuration */
		FchParams_env->Sata.SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
		switch ((SATA_CLASS)CONFIG_STONEYRIDGE_SATA_MODE) {
		case SataRaid:
		case SataAhci:
		case SataAhci7804:
		case SataLegacyIde:
			FchParams_env->Sata.SataIdeMode = FALSE;
			break;
		case SataIde2Ahci:
		case SataIde2Ahci7804:
		default: /* SataNativeIde */
			FchParams_env->Sata.SataIdeMode = TRUE;
			break;
		}

		/* Platform updates */
		platform_FchParams_env(FchParams_env);

		printk(BIOS_DEBUG, "Done\n");
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status = AGESA_UNSUPPORTED;

	if (!ENV_ROMSTAGE)
		return Status;

	if (IS_ENABLED(CONFIG_GENERIC_SPD_BIN)) {
		AGESA_READ_SPD_PARAMS *info = ConfigPtr;
		if (info->MemChannelId > 0)
			return AGESA_UNSUPPORTED;
		if (info->SocketId != 0)
			return AGESA_UNSUPPORTED;
		if (info->DimmId > 1)
			return AGESA_UNSUPPORTED;

		die("SPD in cbfs not yet supported.\n");
	} else {
		Status = AmdMemoryReadSPD(Func, Data, ConfigPtr);
	}

	return Status;
}
