/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 secunet Security Networks AG
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

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <commonlib/helpers.h>
#include <uuid.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_bus.h>
#include <smbios.h>

#include "eeprom.h"

#define MAX_STRING_LENGTH UUID_STRLEN

static struct device *eeprom;

static const char *eeprom_read_string(const enum bx26_strings idx)
{
	static char str[MAX_STRING_LENGTH + 1];

	if (!eeprom) {
		printk(BIOS_WARNING, "DMI: Serial EEPROM not found\n");
		str[0] = '\0';
		return str;
	}

	const size_t offset = bx26_locations[idx].offset;
	const size_t length = MIN(bx26_locations[idx].length, MAX_STRING_LENGTH);

	if (i2c_dev_read_at16(eeprom, (u8 *)str, length, offset) != length) {
		printk(BIOS_WARNING, "DMI: Failed to read serial EEPROM\n");
		str[0] = '\0';
	} else {
		unsigned int i;
		/* Terminate at first non-printable character. */
		for (i = 0; i < length; ++i) {
			if (!isprint(str[i]))
				break;
		}
		str[i] = '\0';
	}

	return str;
}

const char *smbios_system_manufacturer(void)
{
	return eeprom_read_string(SYSTEM_MANUFACTURER);
}

const char *smbios_system_product_name(void)
{
	return eeprom_read_string(SYSTEM_PRODUCT_NAME);
}

const char *smbios_system_serial_number(void)
{
	return eeprom_read_string(SYSTEM_SERIAL_NUMBER);
}

const char *smbios_system_version(void)
{
	return eeprom_read_string(SYSTEM_VERSION);
}

void smbios_system_set_uuid(u8 *const uuid)
{
	if (parse_uuid(uuid, eeprom_read_string(SYSTEM_UUID))) {
		printk(BIOS_WARNING, "DMI: Cannot parse UUID\n");
		memset(uuid, 0x00, UUID_LEN);
	}
}

const char *smbios_mainboard_serial_number(void)
{
	return eeprom_read_string(BOARD_SERIAL_NUMBER);
}

const char *smbios_mainboard_version(void)
{
	return eeprom_read_string(BOARD_VERSION);
}

static void enable_dev(struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_I2C || (dev->path.i2c.device & 0xf0) != 0x50)
		return;
	eeprom = dev;
}

struct chip_operations drivers_secunet_dmi_ops = {
	CHIP_NAME("secunet DMI")
	.enable_dev = enable_dev,
};
