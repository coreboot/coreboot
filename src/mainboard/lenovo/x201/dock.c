/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#if !defined (__PRE_RAM__) && !defined (__SMM__)
#define PCI_DEV(bus, dev, fn) dev_find_slot (bus, PCI_DEVFN (dev, fn))
#endif
#include <delay.h>
#include "dock.h"
#include "southbridge/intel/ibexpeak/pch.h"
#include <ec/acpi/ec.h>

int dock_connect(void)
{
	ec_set_bit(0x02, 0);
	ec_set_bit(0x1a, 0);
	ec_set_bit(0xfe, 4);
	return 0;
}

void dock_disconnect(void)
{
	ec_clr_bit(0x02, 0);
	ec_clr_bit(0x1a, 0);
	ec_clr_bit(0xfe, 4);
}

int dock_present(void)
{
	u16 gpiobase = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;
	u8 st = inb(gpiobase + 0x0c);
	printk(BIOS_DEBUG, "GPIO status is 0x%x\n", st);

	return !((st >> 3) & 1);
}

int dock_ultrabay_device_present(void)
{
	return 0;
}
