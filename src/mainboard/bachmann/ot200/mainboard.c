/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Bachmann electronic GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <device/device.h>
#include <device/smbus.h>
#include <smbios.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <arch/io.h>

/* overwrite a weak function to fill SMBIOS table with a custom value */
static u8 hw_rev = 0;
static char mb_rev_str[2] = { '0' };

const char *smbios_mainboard_version(void)
{
	/* UDMA is not working on all supported devices */
	if (hw_rev < 113) {
		mb_rev_str[0] = '1';
	} else {
		mb_rev_str[0] = '2';
	}

	return mb_rev_str;
}

static void init(struct device *dev)
{
	unsigned int i;
	u32 chksum = 0;
	char block[20];
	msr_t reset;
	device_t eeprom_dev = dev_find_slot_on_smbus(1, 0x52);

	if (eeprom_dev == 0) {
		printk(BIOS_WARNING, "eeprom not found\n");
		return;
	}

	/* turn off all leds except led_ini */
	outb(0x02, 0x5a); /* bit0 - led_run */
			  /* bit1 - led_ini */
			  /* bit2 - led_err */
			  /* bit3-bit7 - write has no effect */
	outb(0x00, 0x49); /* bit0-bit6 - led_7-led_1 */
			  /* bit7 - write has no effect */

	/* read the whole block and check if checksum is okay */
	for (i = 0; i < 20; i++) {
		block[i] = smbus_read_byte(eeprom_dev, i);
		chksum += block[i];
	}

	if (chksum != 0) {
		printk(BIOS_WARNING, "wrong checksum: 0x%0x\n", chksum);
	}

	hw_rev = block[5];

	printk(BIOS_DEBUG, "hw revision: %u\n", hw_rev);

	/* Reset MFGPT7 (standby power domain) - this is done via
	 * an undocumented register */
	reset = rdmsr(0x5140002b);
	reset.lo |= 1 << 7;
	wrmsr(0x5140002b, reset);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
