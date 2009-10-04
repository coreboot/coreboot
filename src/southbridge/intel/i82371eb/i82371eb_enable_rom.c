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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include "i82371eb.h"

static void i82371eb_enable_rom(device_t dev)
{
	u16 reg16;

	/* Enable access to the whole ROM, disable ROM write access. */
	reg16 = pci_read_config16(dev, XBCS);
	reg16 |= LOWER_BIOS_ENABLE;
	reg16 |= EXT_BIOS_ENABLE;
	reg16 |= EXT_BIOS_ENABLE_1MB;
	reg16 &= ~(WRITE_PROTECT_ENABLE);	/* Disable ROM write access. */
	pci_write_config16(dev, XBCS, reg16);
}
