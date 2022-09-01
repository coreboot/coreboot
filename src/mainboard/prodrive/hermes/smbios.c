/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <string.h>
#include <types.h>

#include "eeprom.h"

const char *smbios_system_serial_number(void)
{
	const size_t offset = offsetof(struct eeprom_layout, system_serial_number);
	return eeprom_read_serial(offset, CONFIG_MAINBOARD_SERIAL_NUMBER);
}

const char *smbios_mainboard_serial_number(void)
{
	const size_t offset = offsetof(struct eeprom_layout, board_serial_number);
	return eeprom_read_serial(offset, CONFIG_MAINBOARD_SERIAL_NUMBER);
}

const char *smbios_mainboard_version(void)
{
	static char version_str[8] = { 0 };

	snprintf(version_str, sizeof(version_str), "HSI %u", get_bmc_hsi());

	return version_str;
}
