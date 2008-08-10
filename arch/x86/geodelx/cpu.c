/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2006 Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2006 Andrei Birjukov <andrei.birjukov@artecdesign.ee>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

/* This is a test for the idea of a CPU device. There is only ever going to
 * be one CPU device, the bootstrap processor or BP; other processors will
 * go through a different path. On Geode it is really simple, so we start
 * with that. Later, it gets harder.
 */

#include <console.h>
#include <device/device.h>
#include <lib.h>
#include <io.h>
#include <cpu.h>

/* TODO: Better comment on vsm_end_post_smi, and define 0x05a2 down below. */

/**
 * This is a call to the VSM.
 *
 * TODO: We need to know what it does.
 */
static void vsm_end_post_smi(void)
{
	__asm__ volatile("push %ax\n"
			 "mov $0x5000, %ax\n"
			 ".byte 0x0f, 0x38\n"
			 "pop %ax\n");
}

/**
 * The very last steps in LX init. Turn on caching, tell VSM that we are
 * done. Turn A20 back on in case VSM turned it off.
 *
 * @param dev The device to use.
 */
static void lx_init(struct device *dev)
{
	printk(BIOS_SPEW, "CPU lx_init\n");

	/* Turn on caching if we haven't already. */
	enable_cache();

	/* Do VSA late init. */
	vsm_end_post_smi();

	/* Set gate A20 (legacy VSM disables it in late init). */
	printk(BIOS_SPEW, "A20 (0x92): %d\n", inb(0x92));
	outb(0x02, 0x92);
	printk(BIOS_SPEW, "A20 (0x92): %d\n", inb(0x92));

	printk(BIOS_SPEW, "CPU lx_init DONE\n");
};

/**
 * Device operations for the CPU. 
 *
 * Later, we might need to change it to use a different phase3_scan, and
 * match on a CPU ID. However, CPU IDs are known to be kind of weird,
 * depending on date manufactured they can be all over the place (the Geode
 * alone has had 3 vendors!) so we will have to be careful.
 */
/**
 * The only operations currently set up are the phase 6. We might, however,
 * set up an op in phase3_scan to get the cpuinfo into a struct for all to
 * see. On SMP, it would not be hard to have phase3_scan set up an array of
 * such structs.
 *
 * Further, for systems which have multiple types of CPUs, you can compile
 * in multiple CPU files and use the device ID, at scan time, to pick which
 * one is used. There is a lot of flexibility here!
 */
struct device_operations geodelx_cpuops = {
	{.id = {.type = DEVICE_ID_PCI,
		/* TODO: This is incorrect, these are _not_ PCI IDs! */
		{.pci = {.vendor = X86_VENDOR_AMD,.device = 0x05A2}}},
	 .ops = &geodelx_cpuops}	.constructor	= default_device_constructor,
	.phase3_scan	= NULL,
	.phase6_init	= lx_init,
};
