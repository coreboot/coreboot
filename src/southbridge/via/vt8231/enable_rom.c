/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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

#include <arch/io.h>
#include <device/pci_ids.h>

static void vt8231_enable_rom(void)
{
	device_t dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
	                               PCI_DEVICE_ID_VIA_8231), 0);

	/*
	 * ROM decode control register (0x43):
	 *
	 * Bit  Decode range
	 * -----------------
	 * 7	0xFFFE0000-0xFFFEFFFF
	 * 6	0xFFF80000-0xFFFDFFFF
	 * 5	0xFFF00000-0xFFF7FFFF
	 * 4	0x000E0000-0x000EFFFF
	 * 3	0x000D8000-0x000DFFFF
	 * 2	0x000D0000-0x000D7FFF
	 * 1	0x000C8000-0x000CFFFF
	 * 0	0x000C0000-0x000C7FFF
	 */
	pci_write_config8(dev, 0x43, (1 << 7) | (1 << 6) | (1 << 5));
}
