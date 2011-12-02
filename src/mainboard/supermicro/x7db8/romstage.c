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
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <superio/winbond/w83627hf/early_serial.c>
#include <southbridge/intel/i63xx/i63xx.h>
#include <northbridge/intel/i5000/raminit.h>


static void setup_gpio(void)
{
	pci_write_config32(PCI_DEV(0, 31, 0), 0x48, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCI_DEV(0, 31, 0), 0x4c, (1 << 4));


	outl(0xff0c79cf, DEFAULT_GPIOBASE + 0x00); /* GPIO_USE_SEL */
	outl(0xe700ffff, DEFAULT_GPIOBASE + 0x04); /* GP_IO_SEL */
	outl(0x65bf0000, DEFAULT_GPIOBASE + 0x0c); /* GP_LVL */
	outl(0x0000718a, DEFAULT_GPIOBASE + 0x2c); /* GPI_INV */
	outl(0x00000106, DEFAULT_GPIOBASE + 0x30); /* GPIO_USE_SEL2 */
	outl(0x00000301, DEFAULT_GPIOBASE + 0x34); /* GP_IO_SEL2 */
	outl(0x00030301, DEFAULT_GPIOBASE + 0x38); /* GPIO_LVL2 */

}

static void i5000_lpc_config(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0f);
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

	setup_gpio();

	enable_smbus();

	/* setup PCIe MMCONF base address */
	pci_write_config32(PCI_DEV(0, 16, 0), 0x64,
			   CONFIG_MMCONF_BASE_ADDRESS >> 16);

	outb(0x07, 0x11b8);

	/* These are smbus write captured with serialice. They
	   seem to setup the clock generator */

	if (smbus_write_byte(0x6f, 0x88, 0x1f) ||
	    smbus_write_byte(0x6f, 0x81, 0xff) ||
	    smbus_write_byte(0x6f, 0x82, 0xff) ||
	    smbus_write_byte(0x6f, 0x80, 0x23))
		printk(BIOS_ERR, "Clock generator setup failed\n");

	outb(0x03, 0x11b8);
	outb(0x01, 0x11b8);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xf0, DEFAULT_RCBA | 1);
	i5000_fbdimm_init();
	smbus_write_byte(0x69, 0x01, 0x01);
}
