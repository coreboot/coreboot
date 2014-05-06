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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"

#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/car.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <string.h>
#include <southbridge/amd/agesa/hudson/hudson.h>

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;
#if CONFIG_HAVE_ACPI_RESUME
	void *resume_backup_memory;
#endif
	val = agesawrapper_amdinitmmio();

	hudson_lpc_port80();

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);

		post_code(0x31);
		console_init();
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

	post_code(0x37);
	printk(BIOS_DEBUG, "agesawrapper_amdinitreset ");
	val = agesawrapper_amdinitreset();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitreset failed: %x \n", val);
	}

	post_code(0x39);

	val = agesawrapper_amdinitearly ();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitearly failed: %x \n", val);
	}
	printk(BIOS_DEBUG, "Got past agesawrapper_amdinitearly\n");

#if CONFIG_HAVE_ACPI_RESUME
	if (!acpi_is_wakeup_early()) {		/* Check for S3 resume */
#endif
		post_code(0x40);
		val = agesawrapper_amdinitpost ();
		if(val) {
			printk(BIOS_DEBUG, "agesawrapper_amdinitpost failed: %x \n", val);
		}
		printk(BIOS_DEBUG, "Got past agesawrapper_amdinitpost\n");

		post_code(0x41);
		val = agesawrapper_amdinitenv ();
		if(val) {
			printk(BIOS_DEBUG, "agesawrapper_amdinitenv failed: %x \n", val);
		}
		printk(BIOS_DEBUG, "Got past agesawrapper_amdinitenv\n");
		disable_cache_as_ram();
#if CONFIG_HAVE_ACPI_RESUME
	} else {		/* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		printk(BIOS_DEBUG, "agesawrapper_amdinitresume ");
		val = agesawrapper_amdinitresume();
		if (val)
			printk(BIOS_DEBUG, "error level: %x \n", val);
		else
			printk(BIOS_DEBUG, "passed.\n");

		printk(BIOS_DEBUG, "agesawrapper_amds3laterestore ");
		val = agesawrapper_amds3laterestore ();
		if (val)
			printk(BIOS_DEBUG, "error level: %x \n", val);
		else
			printk(BIOS_DEBUG, "passed.\n");

		post_code(0x61);
		printk(BIOS_DEBUG, "Find resume memory location\n");
		resume_backup_memory = (void *)backup_resume();

		post_code(0x62);
		printk(BIOS_DEBUG, "Move CAR stack.\n");
		move_stack_high_mem();
		printk(BIOS_DEBUG, "stack moved to: 0x%x\n", (u32) (resume_backup_memory + HIGH_MEMORY_SAVE));

		post_code(0x63);
		disable_cache_as_ram();
		printk(BIOS_DEBUG, "CAR disabled.\n");
		set_resume_cache();

		/*
		 * Copy the system memory that is in the ramstage area to the
		 * reserved area.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)(CONFIG_RAMBASE), HIGH_MEMORY_SAVE);

		printk(BIOS_DEBUG, "System memory saved. OK to load ramstage.\n");
	}
#endif

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
