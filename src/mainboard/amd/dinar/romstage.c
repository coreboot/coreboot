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
#include <arch/io.h>
#include <arch/cpu.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/x86/bist.h>
#include <superio/smsc/sch4037/sch4037.h>
#include <superio/smsc/sio1036/sio1036.h>
#include <cpu/x86/lapic.h>
#include "nb_cimx.h"
#include <sb_cimx.h>

#define SERIAL_DEV PNP_DEV(0x4e, SIO1036_SP1)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* Must come first to enable PCI MMCONF. */
	amd_initmmio();

	if (!cpu_init_detectedx && boot_cpu()) {

		post_code(0x30);

		sch4037_early_init(0x2e);

		sio1036_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

		post_code(0x31);
		console_init();

		/*
		 * SR5650/5670/5690 RD890 chipset, read pci config space hang at POR,
		 * Disable all Pcie Bridges to work around It.
		 */
		sr56x0_rd890_disable_pcie_bridge();

	}

	/* Halt if there was a built in self test failure */
	post_code(0x33);
	report_bist_failure(bist);

	// Load MPB
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	if(boot_cpu()) {
		post_code(0x34);
		sb_Poweron_Init();
	}

	post_code(0x35);
	agesawrapper_amdinitreset();

	post_code(0x36);
	agesawrapper_amdinitearly();

	post_code(0x37);
	nb_Poweron_Init();
	post_code(0x38);
	nb_Ht_Init();


	post_code(0x39);
	agesawrapper_amdinitpost();

	post_code(0x40);
	agesawrapper_amdinitenv();


	post_code(0x43);
	printk(BIOS_DEBUG, "Disabling cache as RAM ");
	disable_cache_as_ram();
	printk(BIOS_DEBUG, "done\n");

	post_code(0x44);
	copy_and_run();

	post_code(0x45);  // Should never see this post code.
}
