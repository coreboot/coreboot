/*
 * This file is part of the coreboot project.
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

#define EHCI_BAR_INDEX	0x10
#define EHCI_BAR	0xFEF00000
#define EHCI_DEBUG_OFFSET	0x98

static void set_debug_port(unsigned port)
{
	uint32_t dword;
	dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE+2, 1), 0x74);
	dword &= ~(0xf<<12);
	dword |= (port<<12);
	pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE+2, 1), 0x74, dword);

}

static void mcp55_enable_usbdebug_direct(unsigned port)
{
	set_debug_port(port);
	pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE+2, 1), EHCI_BAR_INDEX, EHCI_BAR);
	pci_write_config8(PCI_DEV(0, MCP55_DEVN_BASE+2, 1), 0x04, 0x2); // mem space enabe
}

