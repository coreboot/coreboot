/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include "chip.h"

/********************************************************
* dbm690t uses a BCM5789 as on-board NIC.
* It has a pin named LOW_POWER to enable it into LOW POWER state.
* In order to run NIC, we should let it out of Low power state. This pin
* is controlled by sb600 GPM3.
* RRG4.2.3 GPM as GPIO
* GPM pins can be used as GPIO. The GPM I/O functions is controlled by three registers:
* I/O C50, C51, C52, PM I/O94, 95, 96.
* RRG4.2.3.1 GPM pins as Input
* RRG4.2.3.2 GPM pins as Output
********************************************************/
static void enable_onboard_nic()
{
	u8 byte;

	printk_info("enable_onboard_nic.\n");

	outb(0x13, 0xC50);

	byte = inb(0xC51);
	byte &= 0x3F;
	byte |= 0x40;
	outb(byte, 0xC51);

	byte = inb(0xC52);
	byte &= ~0x8;
	outb(byte, 0xC52);

	byte = inb(0xC51);
	byte &= 0x3F;
	byte |= 0x80;		/* 7:6=10 */
	outb(byte, 0xC51);

	byte = inb(0xC52);
	byte &= ~0x8;
	outb(byte, 0xC52);
}

/*************************************************
* enable the dedicated function in dbm690t board.
* This function called early than rs690_enable.
*************************************************/
void dbm690t_enable(device_t dev)
{
	struct mainboard_amd_dbm690t_config *mainboard =
	    (struct mainboard_amd_dbm690t_config *)dev->chip_info;

#if (CONFIG_GFXUMA == 1)
	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk_info("dbm690t_enable, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n", msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk_info("dbm690t_enable, TOP MEM2: msr2.lo = 0x%08x, msr2.hi = 0x%08x\n", msr2.lo, msr2.hi);

	switch (msr.lo) {
		case 0x10000000: /* 256M system memory */
			uma_memory_size = 0x2000000; /* 32M recommended UMA */
			break;

		case 0x18000000: /* 384M system memory */
			uma_memory_size = 0x4000000; /* 64M recommended UMA */
			break;

		case 0x20000000: /* 512M system memory */
			uma_memory_size = 0x4000000; /* 64M recommended UMA */
			break;

		default: /* 1GB and above system memory */
			uma_memory_size = 0x8000000; /* 128M recommended UMA */
			break;
	}

	uma_memory_start = msr.lo - uma_memory_size;/* TOP_MEM1 */
	printk_info("dbm690t_enable: uma size 0x%08x, memory start 0x%08x\n", uma_memory_size, uma_memory_start);

	/* TODO: TOP_MEM2 */
#else
	uma_memory_size = 0x8000000; /* 128M recommended UMA */
	uma_memory_start = 0x38000000; /* 1GB  system memory supposed */
#endif

	printk_info("dbm690t_enable. dev=0x%x\n", dev);

	enable_onboard_nic();
}

/*
* CONFIG_CHIP_NAME defined in Option.lb.
*/
struct chip_operations mainboard_amd_dbm690t_ops = {
#if CONFIG_CHIP_NAME == 1
	CHIP_NAME("AMD Dbm690t   Mainboard")
#endif
	.enable_dev = dbm690t_enable,
};
