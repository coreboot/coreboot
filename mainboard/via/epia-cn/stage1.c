/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <io.h>
#include <arch/x86/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <superio/fintek/f71805f/f71805f.h>
#include <northbridge/via/cn700/cn700.h>

/* Someday, we're all going multicore. */
void stop_ap(void)
{
}

void hardware_stage1(void)
{
	void vt1211_enable_serial(u8 dev, u8 serial, u16 iobase);
	u32 dev;

	post_code(POST_START_OF_MAIN);
	vt1211_enable_serial(0x2e, 2, 0x3f8);
	
	/* Enable multifunction for northbridge. */
	pci_conf1_write_config8(0x00, 0x4f, 0x01);

	printk(BIOS_SPEW, "In hardware_stage1()\n");
	/* Disabled GP3, to keep the system from rebooting automatically */
	//pci_conf1_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VT8237R_LPC, &dev);
	dev = PCI_BDF(0, 17, 0);
	pci_conf1_write_config8(dev, 0x98, 0x00);
}

void mainboard_pre_payload(void)
{
	//banner(BIOS_DEBUG, "mainboard_pre_payload: done");
}
