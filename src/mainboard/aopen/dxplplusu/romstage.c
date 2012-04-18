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
#include <arch/romcc_io.h>
#include <arch/cpu.h>
#include <stdlib.h>
#include <console/console.h>
#include <cpu/x86/bist.h>

#include "southbridge/intel/i82801dx/i82801dx.h"
#include "southbridge/intel/i82801dx/early_smbus.c"
#include "southbridge/intel/i82801dx/reset.c"
#include "northbridge/intel/e7505/raminit.h"

#include <device/pnp_def.h>
#include "superio/smsc/lpc47m10x/early_serial.c"


#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	// Get the serial port running and print a welcome banner
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	// Halt if there was a built in self test failure
	report_bist_failure(bist);

	// If this is a warm boot, some initialization can be skipped
	if (!e7505_mch_is_ready()) {
		enable_smbus();

		/* The real MCH initialisation. */
		e7505_mch_init(memctrl);

		/*
		 * ECC scrub invalidates cache, so all stack in CAR
		 * is lost. Only return addresses from main() and
		 * scrub_ecc() are recovered to stack via xmm0-xmm3.
		 */
#if CONFIG_HW_SCRUBBER
		unsigned long ret_addr = (unsigned long)((unsigned long*)&bist - 1);
		e7505_mch_scrub_ecc(ret_addr);
#endif

		/* Hook for post ECC scrub settings and debug. */
		e7505_mch_done(memctrl);
	}

	printk(BIOS_DEBUG, "SDRAM is up.\n");
}
