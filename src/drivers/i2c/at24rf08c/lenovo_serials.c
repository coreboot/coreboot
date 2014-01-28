/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/smbus.h>
#include <smbios.h>
#include <console/console.h>

static void at24rf08c_read_string(u8 bank, u8 start, u8 len, char *result)
{
	int i;
	device_t dev;

	dev = dev_find_slot_on_smbus(1, 0x54 | bank);
	if (dev == 0) {
		printk(BIOS_WARNING, "EEPROM not found\n");
		memcpy(result, "*INVALID*", sizeof ("*INVALID*"));
		return;
	} 

	for (i = 0; i < len; i++) {
		int t;
		int j;
		/* After a register write AT24RF08C (which we issued in init function) sometimes stops responding.
		   Retry several times in case of failure.
		*/
		for (j = 0; j < 100; j++) {
			t = smbus_read_byte(dev, start + i);
			if (t >= 0)
				break;
		}
		if (t < 0x20 || t > 0x7f) {
			memcpy(result, "*INVALID*", sizeof ("*INVALID*"));
			return;
		}
		result[i] = t;
	}
}

const char *smbios_mainboard_serial_number(void)
{
	static char result[12];
	static int already_read;

	if (already_read)
		return result;

	memset(result, 0, sizeof (result));
	at24rf08c_read_string(0, 0x2e, 7, result);

	already_read = 1;
	return result;
}

const char *smbios_mainboard_product_name(void)
{
	static char result[12];
	static int already_read;

	if (already_read)
		return result;

	memset (result, 0, sizeof (result));
	at24rf08c_read_string(0, 0x27, 7, result);

	already_read = 1;
	return result;
}

void smbios_mainboard_set_uuid(u8 *uuid)
{
	static char result[16];
	unsigned i;
	static int already_read;
	device_t dev;
	const int remap[16] = {
		/* UUID byteswap.  */
		3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15
	};


	if (already_read) {
		memcpy (uuid, result, 16);
		return;
	}

	memset (result, 0, sizeof (result));

	dev = dev_find_slot_on_smbus(1, 0x56);
	if (dev == 0) {
		printk(BIOS_WARNING, "eeprom not found\n");
		already_read = 1;
		memset (uuid, 0, 16);
		return;
	}

	for (i = 0; i < 16; i++) {
		int t;
		int j;
		/* After a register write AT24RF08C (which we issued in init function) sometimes stops responding.
		   Retry several times in case of failure.
		*/
		for (j = 0; j < 100; j++) {
			t = smbus_read_byte(dev, 0x12 + i);
			if (t >= 0)
				break;
		}
		if (t < 0) {
			memset (result, 0, sizeof (result));
			break;
		}
		result[remap[i]] = t;
	}

	already_read = 1;

	memcpy (uuid, result, 16);
}
