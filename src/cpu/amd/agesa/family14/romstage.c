/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

#include <arch/acpi.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/bist.h>

#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <string.h>

#include <commonlib/loglevel.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/car.h>
#include <sb_cimx.h>

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sysinfo *cb = NULL;
	u32 val;

	amd_initmmio();

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);
		sb_Poweron_Init();

		post_code(0x31);

		board_BeforeAgesa(cb);

		console_init();
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	post_code(0x37);
	agesawrapper_amdinitreset();

	post_code(0x39);
	agesawrapper_amdinitearly();

	int s3resume = acpi_is_wakeup_s3();
	if (!s3resume) {

		printk(BIOS_INFO, "Normal boot\n");

		post_code(0x40);
		agesawrapper_amdinitpost();

		post_code(0x41);
		agesawrapper_amdinitenv();

		post_code(0x42);
		amd_initenv();

	} else {
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);

		agesawrapper_amdinitresume();

		post_code(0x61);

		agesawrapper_amds3laterestore();

		post_code(0x62);

		prepare_for_resume();
	}

	post_code(0x50);
	copy_and_run();
}

