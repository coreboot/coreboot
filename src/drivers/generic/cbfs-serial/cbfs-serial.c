/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <device/device.h>
#include <smbios.h>
#include <string.h>

#define MAX_SERIAL_LENGTH 0x100

const char *smbios_mainboard_serial_number(void)
{
	static char serial_number[MAX_SERIAL_LENGTH + 1] = {0};

	if (serial_number[0] != 0)
		return serial_number;

	size_t serial_len = cbfs_load("serial_number", serial_number, MAX_SERIAL_LENGTH);
	if (serial_len) {
		serial_number[serial_len] = '\0';
		return serial_number;
	}

	strncpy(serial_number, CONFIG_MAINBOARD_SERIAL_NUMBER,
			MAX_SERIAL_LENGTH);

	return serial_number;
}
