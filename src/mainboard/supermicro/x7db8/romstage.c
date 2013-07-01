/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <superio/winbond/w83627hf/early_serial.c>
#include <northbridge/intel/i5000/raminit.h>
#include "northbridge/intel/i3100/i3100.h"
#include "southbridge/intel/i3100/i3100.h"
#include <southbridge/intel/i3100/early_smbus.c>

#define DEVPRES_CONFIG  (DEVPRES_D1F0 | DEVPRES_D2F0 | DEVPRES_D3F0)
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

#define RCBA_RPC   0x0224 /* 32 bit */
#define RCBA_HPTC  0x3404 /* 32 bit */
#define RCBA_GCS   0x3410 /* 32 bit */
#define RCBA_FD    0x3418 /* 32 bit */

static void early_config(void)
{
	u32 gcs, rpc, fd;

	/* Enable RCBA */
	pci_write_config32(PCI_DEV(0, 0x1F, 0), RCBA, DEFAULT_RCBA | 1);

	/* Disable watchdog */
	gcs = read32(DEFAULT_RCBA + RCBA_GCS);
	gcs |= (1 << 5); /* No reset */
	write32(DEFAULT_RCBA + RCBA_GCS, gcs);

	/* Configure PCIe port B as 4x */
	rpc = read32(DEFAULT_RCBA + RCBA_RPC);
	rpc |= (3 << 0);
	write32(DEFAULT_RCBA + RCBA_RPC, rpc);

	/* Disable Modem, Audio, PCIe ports 2/3/4 */
	fd = read32(DEFAULT_RCBA + RCBA_FD);
	fd |= (1 << 19) | (1 << 18) | (1 << 17) | (1 << 6) | (1 << 5);
	write32(DEFAULT_RCBA + RCBA_FD, fd);

	/* Enable HPET */
	write32(DEFAULT_RCBA + RCBA_HPTC, (1 << 7));

	/* Setup sata mode */
	pci_write_config8(PCI_DEV(0, 0x1F, 2), SATA_MAP, 0x40);
}

#define DEFAULT_GPIOBASE 0x1180
static void setup_gpio(void)
{
	pci_write_config32(PCI_DEV(0, 31, 0), 0x48, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCI_DEV(0, 31, 0), 0x4c, (1 << 4));

	outl(0xff0c79cf, DEFAULT_GPIOBASE + 0x00); /* GPIO_USE_SEL */
	outl(0xe700ffff, DEFAULT_GPIOBASE + 0x04); /* GP_IO_SEL */
	outl(0x65b70000, DEFAULT_GPIOBASE + 0x0c); /* GP_LVL */
	outl(0x0000718a, DEFAULT_GPIOBASE + 0x2c); /* GPI_INV */
	outl(0x00000106, DEFAULT_GPIOBASE + 0x30); /* GPIO_USE_SEL2 */
	outl(0x00000301, DEFAULT_GPIOBASE + 0x34); /* GP_IO_SEL2 */
	outl(0x00030301, DEFAULT_GPIOBASE + 0x38); /* GPIO_LVL2 */
}

static void i5000_lpc_config(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0f);
}

int mainboard_set_fbd_clock(int speed)
{
	switch(speed) {
		case 533:
			smbus_write_byte(0x6f, 0x80, 0x21);
			return 0;
		case 667:
			smbus_write_byte(0x6f, 0x80, 0x23);
			return 0;
		default:
			printk(BIOS_ERR, "Invalid clock: %dMHz\n", speed);
			die("");
			return -1;
	}
}

void main(unsigned long bist)
{
	if (bist == 0)
		enable_lapic();

	i5000_lpc_config();

	w83627hf_enable_serial(PNP_DEV(0x2e, 2), 0x3f8);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	early_config();

	setup_gpio();

	enable_smbus();

	outb(0x07, 0x11b8);

	/* These are smbus write captured with serialice. They
	   seem to setup the clock generator */

	smbus_write_byte(0x6f, 0x88, 0x1f);
	smbus_write_byte(0x6f, 0x81, 0xff);
	smbus_write_byte(0x6f, 0x82, 0xff);
	smbus_write_byte(0x6f, 0x80, 0x23);

	outb(0x03, 0x11b8);
	outb(0x01, 0x11b8);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xf0, DEFAULT_RCBA | 1);
	i5000_fbdimm_init();
	smbus_write_byte(0x69, 0x01, 0x01);
}
