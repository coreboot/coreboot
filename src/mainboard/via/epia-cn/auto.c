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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/via/cn700/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "southbridge/via/vt8237r/vt8237r_early_smbus.c"
#include "southbridge/via/vt8235/vt8235_early_serial.c"

static void memreset_setup(void)
{
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/via/cn700/raminit.c"

static void enable_mainboard_devices(void)
{
	device_t dev;
	u8 reg;

	/*
	 * If I enable SATA, FILO will not find the IDE disk, so I'll disable
	 * SATA here. To not conflict with PCI spec, I'll move IDE device
	 * from 00:0f.1 to 00:0f.0.
	 */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT6420_SATA), 0);
	if (dev != PCI_DEV_INVALID) {
		/* Enable PATA. */
		reg = pci_read_config8(dev, 0xd1);
		reg |= 0x08;
		pci_write_config8(dev, 0xd1, reg);
		reg = pci_read_config8(dev, 0x49);
		reg |= 0x80;
		pci_write_config8(dev, 0x49, reg);
	} else {
		print_debug("No SATA device\r\n");
	}

	/* Disable SATA, and PATA device will be 00:0f.0. */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);
	if (dev == PCI_DEV_INVALID)
		die("Southbridge not found!!!\r\n");
	reg = pci_read_config8(dev, 0x50);
	reg |= 0x08;
	pci_write_config8(dev, 0x50, reg);
}

static const struct mem_controller ctrl = {
	.d0f0 = 0x0000,
	.d0f2 = 0x2000,
	.d0f3 = 0x3000,
	.d0f4 = 0x4000,
	.d0f7 = 0x7000,
	.d1f0 = 0x8000,
	.channel0 = { 0x50 },
};

static void main(unsigned long bist)
{
	unsigned long x;
	device_t dev;

	/* Enable multifunction for northbridge. */
	pci_write_config8(ctrl.d0f0, 0x4f, 0x01);

	enable_vt8235_serial();
	uart_init();
	console_init();

	print_spew("In auto.c:main()\r\n");

	enable_smbus();
	smbus_fixup(&ctrl);

	if (bist == 0) {
		print_debug("doing early_mtrr\r\n");
		early_mtrr_init();
	}

	/* Halt if there was a built-in self test failure. */
	report_bist_failure(bist);

	print_debug("Enabling mainboard devices\r\n");
	enable_mainboard_devices();

	ddr_ram_setup(&ctrl);

	/* ram_check(0, 640 * 1024); */

	print_spew("Leaving auto.c:main()\r\n");
}
