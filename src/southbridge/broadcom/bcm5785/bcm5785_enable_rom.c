/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

static void bcm5785_enable_rom(void)
{
	unsigned char byte;
	device_t addr;

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	/* Locate the BCM 5785 SB PCI Main */
	addr = pci_locate_device(PCI_ID(0x1166, 0x0205), 0); // 0x0201?

	/* Set the 4MB enable bit bit */
	byte = pci_read_config8(addr, 0x41);
	byte |= 0x0e;
	pci_write_config8(addr, 0x41, byte);
}
