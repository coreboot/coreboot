/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <cpu/x86/msr.h>
#include <device/device.h>

/*************************************************
 * enable the dedicated function in thatcher board.
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	msr_t msr;

	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	msr = rdmsr(0xC0011020);
	msr.lo &= ~(1 << 28);
	wrmsr(0xC0011020, msr);

	msr = rdmsr(0xC0011022);
	msr.lo &= ~(1 << 4);
	msr.lo &= ~(1 << 13);
	wrmsr(0xC0011022, msr);

	msr = rdmsr(0xC0011023);
	msr.lo &= ~(1 << 23);
	wrmsr(0xC0011023, msr);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
