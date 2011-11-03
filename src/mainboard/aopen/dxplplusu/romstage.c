/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <stdlib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>

#include "southbridge/intel/i82801dx/i82801dx.h"
#include "southbridge/intel/i82801dx/early_smbus.c"
#include "southbridge/intel/i82801dx/reset.c"
#include "northbridge/intel/e7505/raminit.h"
#include "northbridge/intel/e7505/debug.c"
#include "superio/smsc/lpc47m10x/early_serial.c"

#if !CONFIG_CACHE_AS_RAM
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#endif
#include "cpu/x86/bist.h"

#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/* Cache-As-Ram compiles for this board, but with the CPUs I have,
 * it halts on boot while in Local Apic ID negotiation.
 */

#if CONFIG_CACHE_AS_RAM
#define BOARD_MAIN(x) void main(x)
#define early_mtrr_init()   do {} while (0)
#else
#define BOARD_MAIN(x) static void main(x)
#endif

#include "northbridge/intel/e7505/raminit.c"
#include "northbridge/intel/e7505/reset_test.c"
#include "lib/generic_sdram.c"

// This function MUST appear last (ROMCC limitation)
BOARD_MAIN(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	if (bist == 0) 	{
		// Skip this if there was a built in self test failure
		early_mtrr_init();
	        enable_lapic();
	}

	// Get the serial port running and print a welcome banner
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	// Halt if there was a built in self test failure
	report_bist_failure(bist);

	// If this is a warm boot, some initialization can be skipped
	if (!bios_reset_detected()) {
		enable_smbus();
		sdram_initialize(ARRAY_SIZE(memctrl), memctrl);
	}

	// NOTE: ROMCC dies with an internal compiler error
	//		 if the following line is removed.
	print_debug("SDRAM is up.\r\n");

}
