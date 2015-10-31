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
 */

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <cpu/amd/agesa/s3_resume.h>
#include "cbmem.h"


void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  To be consistent with Parmer, setting to 4mA
	 *  even though the register is not documented in the Kabini BKDG.
	 *  Otherwise the serial output is bad code.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);

	amd_initmmio();

	/* Set LPC decode enables. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

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
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/* On Larne, after LpcClkDrvSth is set, it needs some time to be stable, because of the buffer ICS551M */
	int i;
	for(i = 0; i < 200000; i++)
		val = inb(0xcd6);

	post_code(0x37);
	agesawrapper_amdinitreset();
	post_code(0x38);
	printk(BIOS_DEBUG, "Got past yangtze_early_setup\n");

	post_code(0x39);

	agesawrapper_amdinitearly();
	int s3resume = acpi_is_wakeup_s3();
	if (!s3resume) {
		post_code(0x40);
		agesawrapper_amdinitpost();
		post_code(0x41);
		agesawrapper_amdinitenv();
		/* TODO: Disable cache is not ok. */
		disable_cache_as_ram();
	} else { /* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		agesawrapper_amdinitresume();

		amd_initcpuio();
		agesawrapper_amds3laterestore();

		post_code(0x61);
		prepare_for_resume();
	}

	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
