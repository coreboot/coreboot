/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2014 Advanced Micro Devices, Inc.
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
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include "superio/fintek/f81216h/f81216h.h"

#define SERIAL_DEV PNP_DEV(0x4e, F81216H_SP1)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/*
	 *  In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  This following register setting has been
	 *  replicated in every reference design since Parmer, so it is
	 *  believed to be required even though it is not documented in
	 *  the SoC BKDGs.  Without this setting, there is no serial
	 *  output.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);

	hudson_lpc_decode();

	outb(0x24, 0xCD6);
	outb(0x01, 0xCD7);
	*(volatile u32 *) (AMD_SB_ACPI_MMIO_ADDR + 0xE00 + 0x28) |= 1 << 18; /* 24Mhz */
	*(volatile u32 *) (AMD_SB_ACPI_MMIO_ADDR + 0xE00 + 0x40) &= ~(1 << 2); /* 24Mhz */

	hudson_lpc_port80();

	if (!cpu_init_detectedx) {
		post_code(0x30);
		f81216h_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE, MODE_7777);
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

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);
	post_code(0x38);
	printk(BIOS_DEBUG, "Got past hudson_early_setup\n");

	post_code(0x39);
	AGESAWRAPPER(amdinitearly);

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);
}

void agesa_postcar(struct sysinfo *cb)
{
	post_code(0x41);
	AGESAWRAPPER(amdinitenv);
}
