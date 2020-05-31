/* SPDX-License-Identifier: BSD-3-Clause */

#include <smbios.h>

#include "vpd.h"
#include "vpd_tables.h"

#define VPD_KEY_SYSTEM_SERIAL		"serial_number"
#define VPD_KEY_MAINBOARD_SERIAL	"mlb_serial_number"
#define VPD_SERIAL_LEN			64

const char *smbios_system_serial_number(void)
{
	static char serial[VPD_SERIAL_LEN];
	if (vpd_gets(VPD_KEY_SYSTEM_SERIAL, serial, VPD_SERIAL_LEN, VPD_RO))
		return serial;
	return "";
}

const char *smbios_mainboard_serial_number(void)
{
	static char serial[VPD_SERIAL_LEN];
	if (vpd_gets(VPD_KEY_MAINBOARD_SERIAL, serial, VPD_SERIAL_LEN, VPD_RO))
		return serial;
	return "";
}
