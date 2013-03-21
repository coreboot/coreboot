/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <lib.h>
#include "northbridge/via/cx700/raminit.h"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "northbridge/via/cx700/early_smbus.c"
#include "lib/debug.c"
#include "northbridge/via/cx700/early_serial.c"
#include "northbridge/via/cx700/raminit.c"
#include <spd.h>

static void enable_mainboard_devices(void)
{
	device_t dev;

	dev = pci_locate_device(PCI_ID(0x1106, 0x8324), 0);
	if (dev == PCI_DEV_INVALID) {
		die("LPC bridge not found!!!\n");
	}
	// Disable GP3
	pci_write_config8(dev, 0x98, 0x00);

	// Disable mc97
	pci_write_config8(dev, 0x50, 0x80);

	// Disable internal KBC Configuration
	pci_write_config8(dev, 0x51, 0x2d);
	pci_write_config8(dev, 0x58, 0x42);
	pci_write_config8(dev, 0x59, 0x80);
	pci_write_config8(dev, 0x5b, 0x01);

	// Enable P2P Bridge Header for External PCI BUS.
	dev = pci_locate_device(PCI_ID(0x1106, 0x324e), 0);
	if (dev == PCI_DEV_INVALID) {
		die("P2P bridge not found!!!\n");
	}
	pci_write_config8(dev, 0x4f, 0x41);

	// Switch SATA to non-RAID mode
	dev = pci_locate_device(PCI_ID(0x1106, 0x0581), 0);
	if (dev != PCI_DEV_INVALID) {
		pci_write_config16(dev, 0xBA, 0x5324);
	}
}

static void enable_shadow_ram(const struct mem_controller *ctrl)
{
	u8 shadowreg;

	pci_write_config8(PCI_DEV(0, 0, 3), 0x80, 0x2a);

	/* 0xf0000-0xfffff - ACPI tables */
	shadowreg = pci_read_config8(PCI_DEV(0, 0, 3), 0x83);
	shadowreg |= 0x30;
	pci_write_config8(PCI_DEV(0, 0, 3), 0x83, shadowreg);
}

void main(unsigned long bist)
{
	/* Set statically so it should work with cx700 as well */
	static const struct mem_controller cx700[] = {
		{
			.channel0 = {DIMM0, DIMM1},
		},
	};

	enable_smbus();

	enable_cx700_serial();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	enable_mainboard_devices();

	/* Allows access to all northbridge devices */
	pci_write_config8(PCI_DEV(0, 0, 0), 0x4f, 0x01);

	sdram_set_registers(cx700);
	enable_shadow_ram(cx700);
	sdram_enable(cx700);
}
