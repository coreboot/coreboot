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
#include "northbridge/via/cn400/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "southbridge/via/vt8237r/early_smbus.c"
#include "superio/winbond/w83697hf/early_serial.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83697HF_SP1)
#define DUMMY_DEV PNP_DEV(0x2e, 0)

static const struct mem_controller ctrl = {
	.d0f0 = 0x0000,
	.d0f2 = 0x2000,
	.d0f3 = 0x3000,
	.d0f4 = 0x4000,
	.d0f7 = 0x7000,
	.d1f0 = 0x8000,
	.channel0 = { DIMM0 },
};

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/via/cn400/raminit.c"

static void enable_mainboard_devices(void)
{
	device_t dev;
	u8 reg;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);
	if (dev == PCI_DEV_INVALID)
		die("Southbridge not found!!!\n");

	/* bit=0 means enable function (per VT8237R datasheet)
	 *   7 17.6 MC97
	 *   6 17.5 AC97
	 *   5 16.1 USB 2
	 *   4 16.0 USB 1
	 *   3 15.0 SATA and PATA
	 *   2 16.2 USB 3
	 *   1 16.4 USB EHCI
	 */
	pci_write_config8(dev, 0x50, 0xC0);

	/*bit=0 means enable internal function (per VT8237R datasheet)
	 *   7 USB Device Mode
	 *bit=1 means enable internal function (per VT8237R datasheet)
	 *   6 Reserved
	 *   5 LAN Controller Clock Gating
	 *   4 LAN Controller
	 *   3 Internal RTC
	 *   2 Internal PS2 Mouse
	 *   1 Internal KBC Configuration
	 *   0 Internal Keyboard Controller
	 */
	pci_write_config8(dev, 0x51, 0x9d);
}

static void enable_shadow_ram(void)
{
	unsigned char shadowreg;

	shadowreg = pci_read_config8(ctrl.d0f3, 0x82);
	/* 0xf0000-0xfffff Read/Write*/
	shadowreg |= 0x30;
	pci_write_config8(ctrl.d0f3, 0x82, shadowreg);
}

static void main(unsigned long bist)
{
	unsigned long x;
	device_t dev;

	/* Enable multifunction for northbridge. */
	pci_write_config8(ctrl.d0f0, 0x4f, 0x01);

	w83697hf_set_clksel_48(DUMMY_DEV);
	w83697hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	enable_smbus();
	smbus_fixup(&ctrl);

	/* Halt if there was a built-in self test failure. */
	report_bist_failure(bist);

	print_debug("Enabling mainboard devices\n");
	enable_mainboard_devices();

	print_debug("Enable F-ROM Shadow RAM\n");
	enable_shadow_ram();

	print_debug("Setup CPU Interface\n");
	c3_cpu_setup(ctrl.d0f2);

	ddr_ram_setup();

	if (bist == 0)
		early_mtrr_init();
}
