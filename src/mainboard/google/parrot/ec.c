/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <types.h>
#include <console/console.h>
#include <device/device.h>
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
