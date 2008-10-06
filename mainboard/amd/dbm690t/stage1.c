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

#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <cpu.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <arch/x86/msr.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>

#define SERIAL_IOBASE 0x3f8

void hardware_stage1(void)
{
	void enumerate_ht_chain(void);
	void sb600_pci_port80(void);
	void enable_rs690_dev8(void);
	void sb600_lpc_init(void);
	void it8712f_enable_serial(int unused, u16 iobase);
/*
	int max;
	printk(BIOS_ERR, "Stage1: enable rom ...\n");
	max = ARRAY_SIZE(register_values);
	setup_resource_map(register_values, max);
 */
	/* Nothing special needs to be done to find bus 0 */
	/* Allow the HT devices to be found */
	enumerate_ht_chain();

	/* sb600_lpc_port80(); */
	sb600_pci_port80();
	enable_rs690_dev8();
	sb600_lpc_init();

	/* it8712f_enable_serial does not use its 1st parameter. */
	it8712f_enable_serial(0, SERIAL_IOBASE);
	printk(BIOS_ERR, "Done.\n");
	post_code(POST_START_OF_MAIN);

}

void mainboard_pre_payload(void)
{
	banner(BIOS_DEBUG, "mainboard_pre_payload: done");
}
