/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011, 2017 Advanced Micro Devices, Inc.
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

#include <compiler.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <amdblocks/BiosCallOuts.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <stdlib.h>

#include <amdblocks/agesawrapper.h>
#include <amdlib.h>
#include <amdblocks/dimm_spd.h>
#include "chip.h"
#include <amdblocks/car.h>

void __weak platform_FchParams_reset(
				FCH_RESET_DATA_BLOCK *FchParams_reset) {}

AGESA_STATUS agesa_fch_initreset(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset;
		FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");

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

		/* XHCI configuration */
		if (IS_ENABLED(CONFIG_STONEYRIDGE_XHCI_ENABLE))
			FchParams_env->Usb.Xhci0Enable = TRUE;
		else
			FchParams_env->Usb.Xhci0Enable = FALSE;
		FchParams_env->Usb.Xhci1Enable = FALSE;

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
	uint8_t spd_address;
	int err;
	DEVTREE_CONST struct device *dev;
	DEVTREE_CONST struct soc_amd_stoneyridge_config *conf;
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;

	if (!ENV_ROMSTAGE)
		return AGESA_UNSUPPORTED;

	dev = dev_find_slot(0, DCT_DEVFN);
	if (dev == NULL)
		return AGESA_ERROR;

	conf = dev->chip_info;
	if (conf == NULL)
		return AGESA_ERROR;

	if (info->SocketId >= ARRAY_SIZE(conf->spd_addr_lookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(conf->spd_addr_lookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(conf->spd_addr_lookup[0][0]))
		return AGESA_ERROR;

	spd_address = conf->spd_addr_lookup
		[info->SocketId][info->MemChannelId][info->DimmId];
	if (spd_address == 0)
		return AGESA_ERROR;

	err = mainboard_read_spd(spd_address, (void *)info->Buffer,
				CONFIG_DIMM_SPD_SIZE);

	/* Read the SPD if the mainboard didn't fill the buffer */
	if (err || (*info->Buffer == 0))
		err = sb_read_spd(spd_address, (void *)info->Buffer,
				CONFIG_DIMM_SPD_SIZE);

	if (err)
		return AGESA_ERROR;

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_HaltThisAp(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_HALT_THIS_AP_PARAMS *info = ConfigPtr;
	uint32_t flags = 0;

	if (info->PrimaryCore == TRUE)
		return AGESA_UNSUPPORTED; /* force normal path */
	if (info->ExecWbinvd == TRUE)
		flags |= 1;
	if (info->CacheEn == TRUE)
		flags |= 2;

	ap_teardown_car(flags); /* does not return */

	/* Should never reach here */
	return AGESA_UNSUPPORTED;
}

/* Allow mainboards to fill the SPD buffer */
__weak int mainboard_read_spd(uint8_t spdAddress, char *buf,
						size_t len)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1; /* SPD not read */
}
