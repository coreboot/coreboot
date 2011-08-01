/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "update_ucode.h"
#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <delay.h>

#define MODEL_NANO		0x3
#define MODEL_NANO_3000_B0	0x8
#define MODEL_NANO_3000_B2	0xa
#define MODEL_NANO_3000_B7	0xf

#define MSR_IA32_PERF_STATUS	0x00000198
#define MSR_IA32_PERF_CTL	0x00000199
#define MSR_IA32_MISC_ENABLE	0x000001a0


static u16 nano_vid_to_mv(u8 vid)
{
	return (15000 - 125 * vid) / 10;
}

static void nano_power(void)
{
	msr_t msr;
	/* Enable Powersaver */
	msr = rdmsr(MSR_IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLE, msr);

	/* Enable 6 bit or 7-bit VRM support
	 * If we don't do this, then trying to change the VID or FID will hang
	 * the CPU. This MSR is not documented in VIA docs*/
	msr = rdmsr(0x120e);
	msr.lo |= ( (1<<7) | (1<<4) );
	/* FIXME: set bit 5 for 7-bit, or don't set for 6 bit
	 * This will probably require a config option */
	msr.lo |= (1<<5);
	wrmsr(0x120e, msr);

	/* Get voltage and frequency info */
	msr = rdmsr(MSR_IA32_PERF_STATUS);
	u8 min_fid = (msr.hi >> 24);
	u8 max_fid = (msr.hi >>  8) & 0xff;
	u8 min_vid = (msr.hi >> 16) & 0xff;
	u8 max_vid = (msr.hi >>  0) & 0xff;
	u8 cur_vid = (msr.lo >>  0) & 0xff;
	u8 cur_fid = (msr.lo >>  8) & 0xff;

	printk(BIOS_DEBUG, "MSR %.8x:%.8x\n", msr.hi, msr.lo);
	printk(BIOS_INFO, "FID: %dx (min %dx; max %dx)\n",
	       cur_fid, min_fid, max_fid);
	printk(BIOS_INFO, "VID: %dx (min %dx; max %dx)\n",
	       cur_vid, min_vid, max_vid);

	printk(BIOS_INFO, "Voltage: %dmV (min %dmV; max %dmV)\n",
	       nano_vid_to_mv(cur_vid),
	       nano_vid_to_mv(min_vid),
	       nano_vid_to_mv(max_vid));

	printk(BIOS_INFO, "CPU multiplier: %dx (min %dx; max %dx)\n",
	       (int)((msr.lo >> 8) & 0xff),
	       (int)((msr.hi >> 24) & 0xff), (int)((msr.hi >> 8) & 0xff));

	/* Set highest frequency and VID*/
	msr.lo = msr.hi;
	msr.hi = 0;
	printk(BIOS_DEBUG, "MSR %.8x:%.8x\n", msr.hi, msr.lo);
	wrmsr(MSR_IA32_PERF_CTL, msr);

	/* We could wait while CPU is executing the frequency transition, but
	 * the Nano has 2 PLLs, and will instantly switch once the second PLL
	 * is locked and ready */

	printk(BIOS_DEBUG, "MSR 01a0 hi: 0x%.8x lo: 0x%.8x\n", msr.hi, msr.lo);

	msr = rdmsr(0x120e);
	printk(BIOS_DEBUG, "MSR 120e hi: 0x%.8x lo: 0x%.8x\n", msr.hi, msr.lo);
}

static void nano_init(device_t dev)
{
	print_debug(" Nano POWER:\n");
	struct cpuinfo_x86 c;

	get_fms(&c, dev->device);

	switch (c.x86_model) {
	case MODEL_NANO:
		printk(BIOS_INFO, "VIA Nano");
		break;
	case MODEL_NANO_3000_B0:
		printk(BIOS_INFO, "VIA Nano 3000 rev B0");
		break;
	case MODEL_NANO_3000_B2:
		printk(BIOS_INFO, "VIA Nano 3000 rev B2");
		break;
	case MODEL_NANO_3000_B7:
		printk(BIOS_INFO, "VIA Nano 3000 rev B7");
		break;
	default:
		printk(BIOS_EMERG, "CPU not recognized\n");
	}
	printk(BIOS_INFO, "\n");

	/* This will have been defined as an empty function if microcode
	 * updates were disabled in Kconfig, and no ucode binary will have
	 * been included in the ROM */
	nano_update_ucode();

	/* Aaaah, the usual stuff */
	if(0)nano_power();

	/* Turn on cache */
	x86_enable_cache();

	/* Set up Memory Type Range Registers */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init = nano_init,
};

static struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_CENTAUR, 0x06f3},	// VIA NANO 1000/2000 Series
	{X86_VENDOR_CENTAUR, 0x06f8},	// VIA NANO 3000 rev B0
	{X86_VENDOR_CENTAUR, 0x06fa},	// VIA NANO 3000 rev B2
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};