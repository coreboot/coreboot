/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ron Minnich <rminnich@lanl.gov>
 *
 * Copyright (C) 2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* stage2 - LinuxBIOS RAM-based setup. */

#include <arch/types.h>
#include <string.h>
#include <lar.h>
#include <console.h>
#include <device/device.h>
#include <tables.h>

/**
 * Main function of the DRAM part of LinuxBIOS.
 *
 * LinuxBIOS is divided into pre-DRAM part and DRAM part. The phases before
 * this part are phase 0 and phase 1. This part contains phases x through y.
 *
 * Device Enumeration: in the dev_enumerate() phase.
 *
 * TODO: Check whether this documentation is still correct. Improve it.
 */
int stage2(void)
{
	/* TODO: Add comment. */
	void show_all_devs(void);
	post_code(0x20);
	dev_init();

	/* Console init, also ANYTHING that has to be done 
	 * before printk can be used. 
	 */
	post_code(0x30);
	dev_phase1();
	show_all_devs();

//      printk_notice("LinuxBIOS-%s%s %s booting...\n", 
//              linuxbios_version, linuxbios_extra_version, linuxbios_build);

//      printk_notice("LinuxBIOS-%s%s %s booting...\n", 
//              linuxbios_version, linuxbios_extra_version, linuxbios_build);

	/* Here is where weird stuff like init_timer handling should be
	 * done. This is for ANYTHING that might have to happen before
	 * device enumeration but that needs a printk.
	 */
	post_code(0x40);
	dev_phase2();
	show_all_devs();

	/* Walk physical devices and add any dynamic devices to the
	 * device tree.
	 */
	post_code(0x30);
	dev_root_phase3();
	show_all_devs();

	/* Compute and assign the bus resources. */
	post_code(0x40);
	dev_phase4();
	show_all_devs();

	/* Now actually enable devices on the bus. */
	post_code(0x50);
	dev_root_phase5();
	show_all_devs();

	/* Initialize devices on the bus. */
	post_code(0x60);
	dev_phase6();
	show_all_devs();

	/* TODO: Add comment. */
	post_code(0x70);
	write_tables();
	show_all_devs();

	return 0;
}
