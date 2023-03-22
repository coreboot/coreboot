/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>

#include "vpd.h"

const char *smbios_mainboard_serial_number(void)
{
	return get_emi_eeprom_vpd()->serial_number;
}
