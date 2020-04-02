/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <console/qemu_debugcon.h>
#include <arch/io.h>

static int qemu_debugcon_detected;

void qemu_debugcon_init(void)
{
	int detected = (inb(CONFIG_CONSOLE_QEMU_DEBUGCON_PORT) == 0xe9);
	qemu_debugcon_detected = detected;
	printk(BIOS_INFO, "QEMU debugcon %s [port 0x%x]\n",
	       detected ? "detected" : "not found",
	       CONFIG_CONSOLE_QEMU_DEBUGCON_PORT);
}

void qemu_debugcon_tx_byte(unsigned char data)
{
	if (qemu_debugcon_detected != 0)
		outb(data, CONFIG_CONSOLE_QEMU_DEBUGCON_PORT);
}
