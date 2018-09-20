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
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <stdlib.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <timestamp.h>

#include <southbridge/intel/i82801dx/i82801dx.h>
#include <northbridge/intel/e7505/raminit.h>

#include <device/pnp_def.h>
#include <superio/smsc/lpc47m10x/lpc47m10x.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

void mainboard_romstage_entry(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Get the serial port running and print a welcome banner */
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* If this is a warm boot, some initialization can be skipped */
	if (!e7505_mch_is_ready()) {
		enable_smbus();

		timestamp_add_now(TS_BEFORE_INITRAM);

		/* The real MCH initialisation. */
		e7505_mch_init(memctrl);

		/*
		 * ECC scrub invalidates cache, so all stack in CAR
		 * is lost. Only return addresses from main() and
		 * scrub_ecc() are recovered to stack via xmm0-xmm3.
		 */
#if IS_ENABLED(CONFIG_HW_SCRUBBER)
#if !IS_ENABLED(CONFIG_USBDEBUG_IN_ROMSTAGE)
		unsigned long ret_addr = (unsigned long)((unsigned long*)&bist - 1);
		e7505_mch_scrub_ecc(ret_addr);
#endif
#endif

		/* Hook for post ECC scrub settings and debug. */
		e7505_mch_done(memctrl);

		timestamp_add_now(TS_AFTER_INITRAM);
	}

	printk(BIOS_DEBUG, "SDRAM is up.\n");

	cbmem_recovery(0);
}
