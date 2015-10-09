/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/bootblock_common.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>

void main(void)
{
	bootblock_cpu_init();

	/* Mainboard basic init */
	bootblock_mainboard_init();

#if CONFIG_BOOTBLOCK_CONSOLE
	console_init();
#endif

	bootblock_mmu_init();

	if (init_extra_hardware()) {
		printk(BIOS_ERR, "bootblock_simple: failed to init HW.\n");
	} else {
		run_romstage();
	}
	halt();
}
