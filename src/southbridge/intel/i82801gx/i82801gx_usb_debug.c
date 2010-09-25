/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <usbdebug.h>

// An arbitrary address for the BAR
#define EHCI_BAR		0xFEF00000
// These could be read from DEBUG_BASE (0:1d.7 R 0x5A 16bit)
#define EHCI_BAR_INDEX		0x10

#define EHCI_CONFIG_FLAG	0x40
#define EHCI_PORTSC		0x44
#define EHCI_DEBUG_OFFSET	0xA0

/* Required for successful build, but currently empty. */
void set_debug_port(unsigned int port)
{
	/* Not needed, the ICH* southbridges hardcode physical USB port 1. */
}

static void i82801gx_enable_usbdebug(unsigned int port)
{
	u32 dbgctl;

	pci_write_config32(PCI_DEV(0, 0x1d, 7), EHCI_BAR_INDEX, EHCI_BAR);
	pci_write_config8(PCI_DEV(0, 0x1d, 7), 0x04, 0x2); // Memory Space Enable

	printk(BIOS_DEBUG, "Enabling OWNER_CNT\n");
	dbgctl = read32(EHCI_BAR + EHCI_DEBUG_OFFSET);
	dbgctl |= (1 << 30);
	write32(EHCI_BAR + EHCI_DEBUG_OFFSET, dbgctl);
}

