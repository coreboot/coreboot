/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <lib.h>
#include "northbridge/via/cn700/raminit.h"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "southbridge/via/vt8237r/early_smbus.c"
#include "superio/ite/it8716f/early_serial.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8716F_SP1)

static int spd_read_byte(u16 device, u16 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/via/cn700/raminit.c"

static const struct mem_controller ctrl = {
	.d0f0 = 0x0000,
	.d0f2 = 0x2000,
	.d0f3 = 0x3000,
	.d0f4 = 0x4000,
	.d0f7 = 0x7000,
	.d1f0 = 0x8000,
	.channel0 = { DIMM0 }, /* TODO: CN700 currently only supports 1 DIMM. */
};

void main(unsigned long bist)
{
	/* Enable multifunction for northbridge. */
	pci_write_config8(ctrl.d0f0, 0x4f, 0x01);

	it8716f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	enable_smbus();
	smbus_fixup(&ctrl);
	report_bist_failure(bist);
	ddr_ram_setup(&ctrl);
}
