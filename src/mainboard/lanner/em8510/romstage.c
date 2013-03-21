/*
 * This file is part of the coreboot project.
 *
 * Original take from digitallogic/adl855pc
 *
 * Copyright (C) 2010 Travelping GmbH <info@travelping.com>
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
#include <arch/hlt.h>
#include <stdlib.h>
#include <lib.h>
#include <spd.h>
#include "drivers/pc80/udelay_io.c"
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "southbridge/intel/i82801dx/i82801dx.h"
#include "northbridge/intel/i855/raminit.h"
#include "northbridge/intel/i855/debug.c"
#include "superio/winbond/w83627thg/early_serial.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i855/raminit.c"
#include "northbridge/intel/i855/reset_test.c"

void main(unsigned long bist)
{
	if (bist == 0) {
#if 0
		enable_lapic();
		init_timer();
#endif
	}

        w83627thg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

#if 0
	print_pci_devices();
#endif

	if (!bios_reset_detected()) {
        	enable_smbus();
#if 1
		dump_spd_registers();
		dump_smbus_registers();
#endif
		sdram_set_registers();
		sdram_set_spd_registers();
		sdram_enable();
	}
}
