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

#define MODEL_NANO		0x2
#define MODEL_NANO_3000_B0	0x8
#define MODEL_NANO_3000_B2	0xa

#define MSR_IA32_PERF_STATUS	0x00000198
#define MSR_IA32_PERF_CTL	0x00000199
#define MSR_IA32_MISC_ENABLE	0x000001a0
#define NANO_MYSTERIOUS_MSR	0x120e

static void nano_finish_fid_vid_transition(void)
{

	msr_t msr;
	/* Wait until the power transition ends */
	int cnt = 0;
	do {
		udelay(16);
		msr = rdmsr(MSR_IA32_PERF_STATUS);
		cnt++;
		if (cnt > 128) {
			printk(BIOS_WARNING,
			       "Error while updating multiplier and voltage\n");
			break;
		}
	} while (msr.lo & ((1 << 16) | (1 << 17)));

	/* Print the new FID and Voltage */
	u8 cur_vid = (msr.lo >>  0) & 0xff;
	u8 cur_fid = (msr.lo >>  8) & 0xff;
	printk(BIOS_INFO, "New CPU multiplier: %dx\n", cur_fid);
	printk(BIOS_INFO, "New Voltage ID    : %dx\n", cur_vid);
}

static void nano_set_max_fid_vid(void)
{
	msr_t msr;
	/* Get voltage and frequency info */
	msr = rdmsr(MSR_IA32_PERF_STATUS);
	u8 min_fid = (msr.hi >> 24);
	u8 max_fid = (msr.hi >>  8) & 0xff;
	u8 min_vid = (msr.hi >> 16) & 0xff;
	u8 max_vid = (msr.hi >>  0) & 0xff;
	u8 cur_vid = (msr.lo >>  0) & 0xff;
	u8 cur_fid = (msr.lo >>  8) & 0xff;

	printk(BIOS_INFO, "CPU multiplier: %dx (min %dx; max %dx)\n",
	       cur_fid, min_fid, max_fid);
	printk(BIOS_INFO, "Voltage ID    : %dx (min %dx; max %dx)\n",
	       cur_vid, min_vid, max_vid);

	if( (cur_fid != max_fid) || (cur_vid != max_vid) ) {
		/* Set highest frequency and VID */
		msr.lo = msr.hi;
		msr.hi = 0;
		wrmsr(MSR_IA32_PERF_CTL, msr);
		/* Wait for the transition to complete, otherwise, the CPU
		 * might reset itself repeatedly */
		nano_finish_fid_vid_transition();
	}
	/* As a side note, if we didn't update the microcode by this point, the
	 * second PLL will not lock correctly. The clock will still be provided
	 * by the first PLL, and execution will continue normally, ___until___
	 * the CPU switches PLL. Once that happens we will no longer have a
	 * working clock source, and the CPU will hang
	 * Moral of the story: update the microcode, or don't change FID
	 * This check is handled before calling nano_power() */
}

static void nano_power(void)
{
	msr_t msr;
	/* Enable Powersaver */
	msr = rdmsr(MSR_IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLE, msr);

	/* Enable 6 bit or 7-bit VRM support
	 * This MSR is not documented by VIA docs, other than setting these
	 * bits */
	msr = rdmsr(NANO_MYSTERIOUS_MSR);
	msr.lo |= ( (1<<7) | (1<<4) );
	/* FIXME: Do we have a 6-bit or 7-bit VRM?
	 * set bit [5] for 7-bit, or don't set it for 6 bit VRM
	 * This will probably require a Kconfig option
	 * My board has a 7-bit VRM, so I can't test the 6-bit VRM stuff */
	msr.lo |= (1<<5);
	wrmsr(NANO_MYSTERIOUS_MSR, msr);

	/* Set the maximum frequency and voltage */
	nano_set_max_fid_vid();

	/* Enable TM3 */
	msr = rdmsr(MSR_IA32_MISC_ENABLE);
	msr.lo |= ( (1<<3) | (1<<13) );
	wrmsr(MSR_IA32_MISC_ENABLE, msr);

	u8 stepping = ( cpuid_eax(0x1) ) &0xf;
	if(stepping >= MODEL_NANO_3000_B0) {
		/* Hello Nano 3000. The Terminator needs a CPU upgrade */
		/* Enable C1e, C2e, C3e, and C4e states */
		msr = rdmsr(MSR_IA32_MISC_ENABLE);
		msr.lo |= ( (1<<25) | (1<<26) | (1<<31)); /* C1e, C2e, C3e */
		msr.hi |= (1<<0); /* C4e */
		wrmsr(MSR_IA32_MISC_ENABLE, msr);
	}

	/* Lock on Powersaver */
	msr = rdmsr(MSR_IA32_MISC_ENABLE);
	msr.lo |= (1<<20);
	wrmsr(MSR_IA32_MISC_ENABLE, msr);
}

static void nano_init(device_t dev)
{
	struct cpuinfo_x86 c;

	get_fms(&c, dev->device);

	/* We didn't test this on the Nano 1000/2000 series, so warn the user */
	if(c.x86_mask < MODEL_NANO_3000_B0) {
		printk(BIOS_EMERG, "WARNING: This CPU has not been tested. "
				   "Please report any issues encountered. \n");
	}
	switch (c.x86_mask) {
	case MODEL_NANO:
		printk(BIOS_INFO, "VIA Nano");
		break;
	case MODEL_NANO_3000_B0:
		printk(BIOS_INFO, "VIA Nano 3000 rev B0");
		break;
	case MODEL_NANO_3000_B2:
		printk(BIOS_INFO, "VIA Nano 3000 rev B2");
		break;
	default:
		printk(BIOS_EMERG, "Stepping not recognized: %x\n", c.x86_mask);
	}
	printk(BIOS_INFO, "\n");

	/* We only read microcode from CBFS. If we don't have any microcode in
	 * CBFS, we'll just get back with 0 updates. User choice FTW. */
	unsigned int n_updates = nano_update_ucode();

	if(n_updates != 0){
		nano_power();
	} else {
		/* Changing the frequency or voltage without first updating the
		 * microcode will hang the CPU, so just don't do it */
		printk(BIOS_EMERG, "WARNING: CPU Microcode not updated.\n"
		"    Will not change frequency, as this may hang the CPU.\n");
	}

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
	{X86_VENDOR_CENTAUR, 0x06f2},	// VIA NANO 1000/2000 Series
	{X86_VENDOR_CENTAUR, 0x06f8},	// VIA NANO 3000 rev B0
	{X86_VENDOR_CENTAUR, 0x06fa},	// VIA NANO 3000 rev B2
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};
