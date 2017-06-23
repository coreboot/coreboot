/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/bist.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>
#include <soc/hudson.h>

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	amd_initmmio();
	hudson_lpc_port80();
	hudson_lpc_decode();

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);

		if (IS_ENABLED(CONFIG_STONEYRIDGE_UART))
			configure_hudson_uart();

		post_code(0x31);
		console_init();
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	/* Mask bit 31. One result of Silicon Observation */
	report_bist_failure(bist & 0x7FFFFFFF);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);
	post_code(0x38);
	printk(BIOS_DEBUG, "Got past agesawrapper_amdinitreset\n");

	post_code(0x39);
	AGESAWRAPPER(amdinitearly);

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);
	post_code(0x41);
	AGESAWRAPPER(amdinitenv);
	/* TODO: Disable cache is not ok. */
	disable_cache_as_ram();

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
