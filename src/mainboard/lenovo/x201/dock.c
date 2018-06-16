/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <delay.h>
#include "dock.h"
#include <southbridge/intel/ibexpeak/pch.h>
#include "ec/lenovo/h8/h8.h"
#include <ec/acpi/ec.h>

void h8_mainboard_init_dock (void)
{
	if (dock_present()) {
		printk(BIOS_DEBUG, "dock is connected\n");
		dock_connect();
	} else
		printk(BIOS_DEBUG, "dock is not connected\n");
}

void dock_connect(void)
{
	u16 gpiobase = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;

	ec_set_bit(0x02, 0);
	ec_set_bit(0x1a, 0);
	ec_set_bit(0xfe, 4);

	outl(inl(gpiobase + 0x0c) | (1 << 28), gpiobase + 0x0c);
}

void dock_disconnect(void)
{
	u16 gpiobase = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;

	ec_clr_bit(0x02, 0);
	ec_clr_bit(0x1a, 0);
	ec_clr_bit(0xfe, 4);

	outl(inl(gpiobase + 0x0c) & ~(1 << 28), gpiobase + 0x0c);
}

int dock_present(void)
{
	u16 gpiobase = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;
	u8 st = inb(gpiobase + 0x0c);

	return ((st >> 3) & 7) != 7;
}
