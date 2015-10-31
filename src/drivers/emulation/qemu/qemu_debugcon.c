/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Red Hat Inc.
 * Written by Gerd Hoffmann <kraxel@redhat.com>
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

#include <console/console.h>
#include <console/qemu_debugcon.h>
#include <arch/io.h>
#include <arch/early_variables.h>

static int qemu_debugcon_detected CAR_GLOBAL;

void qemu_debugcon_init(void)
{
	int detected = (inb(CONFIG_CONSOLE_QEMU_DEBUGCON_PORT) == 0xe9);
	car_set_var(qemu_debugcon_detected, detected);
	printk(BIOS_INFO, "QEMU debugcon %s [port 0x%x]\n",
	       detected ? "detected" : "not found",
	       CONFIG_CONSOLE_QEMU_DEBUGCON_PORT);
}

void qemu_debugcon_tx_byte(unsigned char data)
{
	if (car_get_var(qemu_debugcon_detected) != 0)
		outb(data, CONFIG_CONSOLE_QEMU_DEBUGCON_PORT);
}
