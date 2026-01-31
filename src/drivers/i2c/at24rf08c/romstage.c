/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>
#include <console/console.h>
#include <drivers/i2c/at24rf08c/lenovo.h>

void lenovo_mainboard_eeprom_lock(void)
{
	printk(BIOS_DEBUG, "Locking EEPROM RFID\n");

	for (int i = 0; i < 8; i++) {
		/* After a register write AT24RF08C sometimes stops responding.
		   Retry several times in case of failure. */
		for (int j = 0; j < 100; j++)
			if (smbus_write_byte(0x5c, i, 0x0f) >= 0)
				break;
	}
}
