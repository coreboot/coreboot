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
#include <amdblocks/acpimmio.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <superio/fintek/f81216h/f81216h.h>

#define SERIAL_DEV PNP_DEV(0x4e, F81216H_SP1)

static void romstage_main_template(void)
{
	misc_write32(0x28, misc_read32(0x28) | (1 << 18)); /* 24Mhz */
	misc_write32(0x40, misc_read32(0x40) & (~(1 << 2))); /* 24Mhz */

	if (!cpu_init_detectedx) {
		post_code(0x30);
		f81216h_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE, MODE_7777);
		post_code(0x31);
		console_init();
	}
}
