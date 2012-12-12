/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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

#include <arch/acpi.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <types.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <ec/compal/ene932/ec.h>
#include "ec.h"


void parrot_ec_init(void)
{
	printk(BIOS_DEBUG, "Parrot EC Init\n");

	/* Clean up the buffers. We don't know the initial condition. */
	kbc_cleanup_buffers();

	/* Report EC info */
	/* EC version: cmd 0x51 - returns three bytes */
	ec_kbc_write_cmd(0x51);
	printk(BIOS_DEBUG,"  EC version %x.%x.%x\n",
		   ec_kbc_read_ob(), ec_kbc_read_ob(), ec_kbc_read_ob());

	/* EC Project name: cmd 0x52, 0xA0 - returns five bytes */
	ec_kbc_write_cmd(0x52);
	ec_kbc_write_ib(0xA0);
	printk(BIOS_DEBUG,"  EC Project: %c%c%c%c%c\n",
		   ec_kbc_read_ob(),ec_kbc_read_ob(),ec_kbc_read_ob(),
		   ec_kbc_read_ob(), ec_kbc_read_ob());

	/* Print the hardware revision */
	printk(BIOS_DEBUG,"  Parrot Revision %x\n", parrot_rev());

	/* US Keyboard */
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE5);

	/* Enable IRQ1 */
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xD1);

	/* TODO - Do device detection and device maintain state (nvs) */
	/* Enable Wireless and Bluetooth */
	ec_kbc_write_cmd(0x45);
	ec_kbc_write_ib(0xAD);

	/* Set Wireless and Bluetooth Available */
	ec_kbc_write_cmd(0x45);
	ec_kbc_write_ib(0xA8);

	/* Set Wireless and Bluetooth Enable */
	ec_kbc_write_cmd(0x45);
	ec_kbc_write_ib(0xA2);
}


/* Parrot Hardware Revision */
u8 parrot_rev(void)
{
	ec_kbc_write_cmd(0x45);
	ec_kbc_write_ib(0x40);
	return ec_kbc_read_ob();
}
