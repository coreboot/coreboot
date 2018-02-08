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
 */

#include <console/console.h>
#include <device/device.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <arch/io.h>

static void vsm_end_post_smi(void)
{
	__asm__ volatile ("push %ax\n"
			  "mov $0x5000, %ax\n"
			  ".byte 0x0f, 0x38\n" "pop %ax\n");
}

static void geode_lx_init(struct device *dev)
{
	printk(BIOS_DEBUG, "geode_lx_init\n");

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Enable the local CPU APICs */
	//setup_lapic();

	// do VSA late init
	vsm_end_post_smi();

	// Set gate A20 (legacy vsm disables it in late init)
	printk(BIOS_DEBUG, "A20 (0x92): %d\n", inb(0x92));
	outb(0x02, 0x92);
	printk(BIOS_DEBUG, "A20 (0x92): %d\n", inb(0x92));

	printk(BIOS_DEBUG, "CPU geode_lx_init DONE\n");
};

static struct device_operations cpu_dev_ops = {
	.init = geode_lx_init,
};

static const struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_AMD, 0x05A2},
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};
