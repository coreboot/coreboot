/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation
 * Copyright (C) 2017 - 2018 Online SAS.
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
 */

#include <string.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/ramstage.h>
#include <smbios.h>

#include "bmcinfo.h"

void mainboard_silicon_init_params(FSPS_UPD *params)
{
	/* Disable eMMC */
	params->FspsConfig.PcdEnableEmmc = 0;

	if (bmcinfo_disable_nic1())
		params->FspsConfig.PcdEnableGbE = 2; // disable lan 1 only
}

/* Override smbios_mainboard_serial_number to retrieve it from BMC */
const char *smbios_mainboard_serial_number(void)
{
	const char *bmc_serial = bmcinfo_serial();
	if (bmc_serial)
		return bmc_serial;
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

/* Override smbios_mainboard_set_uuid */
void smbios_mainboard_set_uuid(u8 *uuid)
{
	const u8 *bmc_uuid = bmcinfo_uuid();
	if (bmc_uuid)
		memcpy(uuid, bmc_uuid, 16);
	/* leave all zero */
}

/* Override smbios_mainboard_version */
const char *smbios_mainboard_version(void)
{
	const int hwRev = bmcinfo_hwrev();
	switch (hwRev) {
	case 0:
		return "Z0";
	case 1:
		return "A0";
	case 2:
		return "A1";
	}
	return "";
}

/* Override smbios_mainboard_features_flags */
u8 smbios_mainboard_feature_flags(void)
{
	return 0xc;
}

/* Override smbios_mainboard_location_in_chassis */
const char *smbios_mainboard_location_in_chassis(void)
{
	static char location[4] = "n/a";
	int slot = bmcinfo_slot();
	if (slot >= 0)
		snprintf(location, 4, "N%d", slot);
	return location;
}

/* Override smbios_mainboard_board_type */
smbios_board_type smbios_mainboard_board_type(void)
{
	return SMBIOS_BOARD_TYPE_SERVER_BLADE;
}
