/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include <console/console.h>
#include <lib.h>
#include <cpu/x86/lapic.h>
#include "pc80/udelay_io.c"
//#include "lib/delay.c"
#include "cpu/x86/bist.h"

//#include "southbridge/intel/i82801ex/i82801ex.h"
#include "southbridge/intel/i82801ex/early_smbus.c"
#include "northbridge/intel/i865/debug.c"
#include "northbridge/intel/i865/raminit.c"
//#include "northbridge/intel/i865/reset_test.c"
#include "northbridge/intel/i865/i865.h"
#include "superio/winbond/w83627hf/early_serial.c"
#include "watchdog.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

//int spd_read_byte(unsigned int device, unsigned int address)
//static inline int spd_read_byte(unsigned device, unsigned address)
//{
//	return smbus_read_byte(device, address);
//}

void main(unsigned long bist) {

	// TODO
	// copied from dell/s1850/romstage.c
	/*
	static const struct mem_controller memctrl[] = {
		{
			// node id?
			.channel0 = { DIMM0, DIMM1, DIMM2, DIMM3, },
			.channel1 = { DIMM4, DIMM5, DIMM6, DIMM7, },
		}
	};
*/
	if (bist == 0)
		enable_lapic();

	w83627hf_set_clksel_48(SERIAL_DEV);
	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	report_bist_failure(bist);

//	if (!bios_reset_detected()) /* board doesn't boot when using bios_rest_detected() */
//	{
		enable_smbus();
		dump_spd_registers();

		disable_ich5_watchdog();

		// TODO: dual-channel, e.g.:
		// sdram_initialize(ARRAY_SIZE(memctrl), memctrl); // copied from dell/s1850/romstage.c
		sdram_initialize();
//	}
}
