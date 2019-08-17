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
#include <cbmem.h>
#include <console/console.h>
#include <arch/romstage.h>

#include <southbridge/intel/i82801dx/i82801dx.h>
#include <northbridge/intel/e7505/raminit.h>

int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

void mainboard_romstage_entry(void)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	/* If this is a warm boot, some initialization can be skipped */
	if (!e7505_mch_is_ready()) {
		enable_smbus();

		/* The real MCH initialisation. */
		e7505_mch_init(memctrl);

		/* Hook for post ECC scrub settings and debug. */
		e7505_mch_done(memctrl);
	}

	printk(BIOS_DEBUG, "SDRAM is up.\n");

	cbmem_recovery(0);
}
