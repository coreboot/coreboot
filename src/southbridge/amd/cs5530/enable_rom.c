/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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
#include "cs5530.h"

static void cs5530_enable_rom(void)
{
	uint8_t reg8;

	/* So far all CS5530(A) ISA bridges we've seen are at 00:12.0. */
	device_t dev = PCI_DEV(0, 0x12, 0);

	/*
	 * Decode 0x000E0000-0x000FFFFF (128 KB), not just 64 KB, and
	 * decode 0xFF000000-0xFFFFFFFF (16 MB), not just 256 KB.
	 *
	 * Make the ROM write-protected.
	 */
	reg8 = pci_read_config8(dev, ROM_AT_LOGIC_CONTROL_REG);
	reg8 |= LOWER_ROM_ADDRESS_RANGE;
	reg8 |= UPPER_ROM_ADDRESS_RANGE;
	reg8 &= ~ROM_WRITE_ENABLE;
	pci_write_config8(dev, ROM_AT_LOGIC_CONTROL_REG, reg8);

	/* Set positive decode on ROM. */
	reg8 = pci_read_config8(dev, DECODE_CONTROL_REG2);
	reg8 |= BIOS_ROM_POSITIVE_DECODE;
	pci_write_config8(dev, DECODE_CONTROL_REG2, reg8);
}
