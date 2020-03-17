/*
 * This file is part of the coreboot project.
 *
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
#include <device/device.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>

/*************************************************
 * enable the dedicated function in thatcher board.
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	msr_t msr;

	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	msr = rdmsr(LS_CFG_MSR);
	msr.lo &= ~(1 << 28);
	wrmsr(LS_CFG_MSR, msr);

	msr = rdmsr(DC_CFG_MSR);
	msr.lo &= ~(1 << 4);
	msr.lo &= ~(1 << 13);
	wrmsr(DC_CFG_MSR, msr);

	msr = rdmsr(BU_CFG_MSR);
	msr.lo &= ~(1 << 23);
	wrmsr(BU_CFG_MSR, msr);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
