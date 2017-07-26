/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation..
 * Copyright (C) 2017 Advanced Micro Devices
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

#include <stdint.h>
#include <console/console.h>
#include <smp/node.h>
#include <bootblock_common.h>
#include <agesawrapper.h>
#include <agesawrapper_call.h>
#include <soc/hudson.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/*
	 * Call lib/bootblock.c main with BSP, shortcut for APs
	 *  todo: rearchitect AGESA entry points to remove need
	 *        to run amdinitreset, amdinitearly from bootblock.
	 *        Remove AP shortcut.
	 */
	if (!boot_cpu())
		bootblock_soc_early_init(); /* APs will not return */

	bootblock_main_with_timestamp(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	amd_initmmio();

	if (!boot_cpu())
		bootblock_soc_init(); /* APs will not return */

	bootblock_fch_early_init();

	post_code(0x90);
	if (CONFIG_STONEYRIDGE_UART)
		configure_hudson_uart();
}

void bootblock_soc_init(void)
{
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);

	post_code(0x38);
	AGESAWRAPPER(amdinitearly); /* APs will not exit amdinitearly */
}
