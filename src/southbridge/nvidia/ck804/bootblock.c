/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>

#if CONFIG_HT_CHAIN_END_UNITID_BASE < CONFIG_HT_CHAIN_UNITID_BASE
#define CK804_DEVN_BASE CONFIG_HT_CHAIN_END_UNITID_BASE
#else
#define CK804_DEVN_BASE CONFIG_HT_CHAIN_UNITID_BASE
#endif

static void ck804_enable_rom(void)
{
	unsigned char byte;
	device_t addr;

	/* Enable 4MB ROM access at 0xFFC00000 - 0xFFFFFFFF. */
	/* Locate the ck804 LPC. */
	addr = PCI_DEV(0, (CK804_DEVN_BASE + 1), 0);

	/* Set the 4MB enable bit. */
	byte = pci_read_config8(addr, 0x88);
	byte |= 0x80;
	pci_write_config8(addr, 0x88, byte);
}

static void bootblock_southbridge_init(void)
{
	ck804_enable_rom();
}
