/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@smittys.pointclark.net>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define ASSEMBLY 1

#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "pc80/udelay_io.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "superio/smsc/smscsuperio/smscsuperio_early_serial.c"
#include "northbridge/intel/i82830/raminit.h"
#include "northbridge/intel/i82830/memory_initialized.c"
#include "southbridge/intel/i82801xx/i82801xx.h"
#include "southbridge/intel/i82801xx/i82801xx_reset.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "spd_table.h"
#include "gpio.c"

#define SERIAL_DEV PNP_DEV(0x2e, SMSCSUPERIO_SP1)

#include "southbridge/intel/i82801xx/i82801xx_early_smbus.c"
#include "southbridge/intel/i82801xx/i82801xx_early_lpc.c"

/**
 * The onboard 128MB PC133 memory does not have a SPD EEPROM so the
 * values have to be set manually, the SO-DIMM socket is located in
 * socket0 (0x50), and the onboard memory is located in socket1 (0x51).
 */
static inline int spd_read_byte(unsigned device, unsigned address)
{
	int i;

	if (device == 0x50) {
		return smbus_read_byte(device, address);
	} else if (device == 0x51) {
		for (i = 0; i < ARRAY_SIZE(spd_table); i++) {
			if (spd_table[i].address == address)
				return spd_table[i].data;
		}
		return 0xFF; /* Return 0xFF when address is not found. */
	} else {
		return 0xFF; /* Return 0xFF on any failures. */
	}
}

#include "northbridge/intel/i82830/raminit.c"
#include "sdram/generic_sdram.c"

/**
 * The AC'97 Audio Controller I/O space registers are read only by default
 * so we need to enable them by setting register 0x41 to 0x01.
 */
static void ac97_io_enable(void)
{
	device_t dev;

	/* Set the ac97 audio device staticly. */
	dev = PCI_DEV(0x0, 0x1f, 0x5);

	/* Enable access to the IO space. */
	pci_write_config8(dev, 0x41, 0x01);
}

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.channel0 = {0x50, 0x51},
		}
	};

	if (bist == 0)
		early_mtrr_init();
		if (memory_initialized()) {
			hard_reset();
		}

	smscsuperio_enable_serial(SERIAL_DEV, TTYS0_BASE);
	mb_gpio_init();
	uart_init();
	console_init();

	enable_smbus();

	/* Prevent the TCO timer from rebooting us */
	i82801xx_halt_tco_timer();

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);

	sdram_set_registers(memctrl);
	sdram_set_spd_registers(memctrl);
	sdram_enable(0, memctrl);

	/* Check RAM. */
	/* ram_check(0, 640 * 1024); */
	/* ram_check(130048 * 1024, 131072 * 1024); */

	ac97_io_enable();
}
