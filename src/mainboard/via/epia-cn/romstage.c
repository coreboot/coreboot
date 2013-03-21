/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
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
#include <console/console.h>
#include <lib.h>
#include "northbridge/via/cn700/raminit.h"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "southbridge/via/vt8237r/early_smbus.c"
#include "southbridge/via/vt8235/early_serial.c"
#include <spd.h>

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/via/cn700/raminit.c"

static void enable_mainboard_devices(void)
{
	device_t dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);
	if (dev == PCI_DEV_INVALID)
		die("Southbridge not found!!!\n");

	/* bit=0 means enable function (per CX700 datasheet)
	 *   5 16.1 USB 2
	 *   4 16.0 USB 1
	 *   3 15.0 SATA and PATA
	 *   2 16.2 USB 3
	 *   1 16.4 USB EHCI
	 */
	pci_write_config8(dev, 0x50, 0x80);

	/* bit=1 means enable internal function (per CX700 datasheet)
	 *   3 Internal RTC
	 *   2 Internal PS2 Mouse
	 *   1 Internal KBC Configuration
	 *   0 Internal Keyboard Controller
	 */
	pci_write_config8(dev, 0x51, 0x1d);
}

static const struct mem_controller ctrl = {
	.d0f0 = 0x0000,
	.d0f2 = 0x2000,
	.d0f3 = 0x3000,
	.d0f4 = 0x4000,
	.d0f7 = 0x7000,
	.d1f0 = 0x8000,
	.channel0 = { DIMM0 },
};

void main(unsigned long bist)
{
	/* Enable multifunction for northbridge. */
	pci_write_config8(ctrl.d0f0, 0x4f, 0x01);

	enable_vt8235_serial();
	console_init();
	enable_smbus();
	smbus_fixup(&ctrl);
	report_bist_failure(bist);
	enable_mainboard_devices();
	ddr_ram_setup(&ctrl);
}
