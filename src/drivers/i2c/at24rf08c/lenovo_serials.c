/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/smbus.h>
#include <smbios.h>
#include <console/console.h>
#include <version.h>
#include "lenovo.h"

#define ERROR_STRING "*INVALID*"

static struct device *at24rf08c_find_bank(u8 bank)
{
	struct device *dev;
	dev = dev_find_slot_on_smbus(1, 0x54 | bank);
	if (!dev)
		printk(BIOS_WARNING, "EEPROM not found\n");
	return dev;
}

static int at24rf08c_read_byte(struct device *dev, u8 addr)
{
	int t = -1;
	int j;

	/* After a register write AT24RF08C (which we issued in init function)
	   sometimes stops responding. Retry several times in case of failure.
	*/
	for (j = 0; j < 100; j++) {
		t = smbus_read_byte(dev, addr);
		if (t >= 0)
			return t;
	}

	return t;
}

static void at24rf08c_read_string_dev(struct device *dev, u8 start,
				      u8 len, char *result)
{
	int i;
	for (i = 0; i < len; i++) {
		int t = at24rf08c_read_byte(dev, start + i);

		if (t < 0x20 || t > 0x7f) {
			memcpy(result, ERROR_STRING, sizeof(ERROR_STRING));
			return;
		}
		result[i] = t;
	}
	result[len] = '\0';
}

static void at24rf08c_read_string(u8 bank, u8 start, u8 len, char *result)
{
	struct device *dev;

	dev = at24rf08c_find_bank(bank);
	if (dev == NULL) {
		printk(BIOS_WARNING, "EEPROM not found\n");
		memcpy(result, ERROR_STRING, sizeof(ERROR_STRING));
		return;
	}

	at24rf08c_read_string_dev(dev, start, len, result);
}

const char *smbios_mainboard_serial_number(void)
{
	static char result[12];
	static int already_read;

	if (already_read)
		return result;

	memset(result, 0, sizeof(result));
	at24rf08c_read_string(0, 0x2e, 7, result);

	already_read = 1;
	return result;
}

const char *lenovo_mainboard_partnumber(void)
{
	static char result[12];
	static int already_read;

	if (already_read)
		return result;

	memset(result, 0, sizeof(result));
	at24rf08c_read_string(0, 0x27, 7, result);

	already_read = 1;
	return result;
}

const char *smbios_mainboard_product_name(void)
{
	return lenovo_mainboard_partnumber();
}

void smbios_system_set_uuid(u8 *uuid)
{
	static char result[16];
	unsigned int i;
	static int already_read;
	struct device *dev;
	const int remap[16] = {
		/* UUID byteswap.  */
		3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15
	};

	if (already_read) {
		memcpy(uuid, result, 16);
		return;
	}

	memset(result, 0, sizeof(result));

	dev = dev_find_slot_on_smbus(1, 0x56);
	if (dev == NULL) {
		printk(BIOS_WARNING, "EEPROM not found\n");
		already_read = 1;
		memset(uuid, 0, 16);
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
			memset(result, 0, sizeof(result));
			break;
		}
		result[remap[i]] = t;
	}

	already_read = 1;

	memcpy(uuid, result, 16);
}

const char *smbios_mainboard_version(void)
{
	static char result[100];
	static int already_read;
	struct device *dev;
	int len;

	if (already_read)
		return result;

	memset(result, 0, sizeof(result));

	dev = at24rf08c_find_bank(2);
	if (dev == NULL) {
		memcpy(result, ERROR_STRING, sizeof(ERROR_STRING));
		return result;
	}

	len = at24rf08c_read_byte(dev, 0x26) - 2;
	if (len < 0 || len > sizeof(result) - 1) {
		memcpy(result, ERROR_STRING, sizeof(ERROR_STRING));
		return result;
	}

	at24rf08c_read_string_dev(dev, 0x27, len, result);

	already_read = 1;
	return result;
}

const char *smbios_mainboard_bios_version(void)
{
	static char *s = NULL;

	/* Satisfy thinkpad_acpi.  */
	if (strlen(CONFIG_LOCALVERSION))
		return "CBET4000 " CONFIG_LOCALVERSION;

	if (s != NULL)
		return s;
	s = strconcat("CBET4000 ", coreboot_version);
	return s;
}

const char *smbios_mainboard_manufacturer(void)
{
	return "LENOVO";
}
