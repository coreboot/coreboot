/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#if HT_CHAIN_END_UNITID_BASE != 0x20
	#define MCP55_DEVN_BASE	HT_CHAIN_END_UNITID_BASE
#else
	#define MCP55_DEVN_BASE	HT_CHAIN_UNITID_BASE
#endif

static void mcp55_enable_rom(void)
{
	uint8_t byte;
	uint16_t word;
	device_t addr;

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
#if 0
	/*	default MCP55 LPC single */
	addr = pci_locate_device(PCI_ID(0x10de, 0x0367), 0);
#else
//	addr = pci_locate_device(PCI_ID(0x10de, 0x0360), 0);
	addr = PCI_DEV(0, (MCP55_DEVN_BASE+1), 0);
#endif

	/* Set the 4MB enable bit bit */
	byte = pci_read_config8(addr, 0x88);
	byte |= 0xff; //256K
	pci_write_config8(addr, 0x88, byte);
	byte = pci_read_config8(addr, 0x8c);
	byte |= 0xff; //1M
	pci_write_config8(addr, 0x8c, byte);
	word = pci_read_config16(addr, 0x90);
	word |= 0x7fff; //15M
	pci_write_config16(addr, 0x90, word);
}
