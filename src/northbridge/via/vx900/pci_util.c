/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vx900.h"

void dump_pci_device(device_t dev)
{
	int i;
	for (i = 0; i <= 0xff; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%.2x:", i);

		if ((i & 0x0f) == 0x08)
			printk(BIOS_DEBUG, " |");

		val = pci_read_config8(dev, i);
		printk(BIOS_DEBUG, " %.2x", val);

		if ((i & 0x0f) == 0x0f)
			printk(BIOS_DEBUG, "\n");
	}
}

void pci_mod_config8(device_t dev, unsigned int where,
		     uint8_t clr_mask, uint8_t set_mask)
{
	uint8_t reg8 = pci_read_config8(dev, where);
	reg8 &= ~clr_mask;
	reg8 |= set_mask;
	pci_write_config8(dev, where, reg8);
}

void pci_mod_config16(device_t dev, unsigned int where,
		      uint16_t clr_mask, uint16_t set_mask)
{
	uint16_t reg16 = pci_read_config16(dev, where);
	reg16 &= ~clr_mask;
	reg16 |= set_mask;
	pci_write_config16(dev, where, reg16);
}

void pci_mod_config32(device_t dev, unsigned int where,
		      uint32_t clr_mask, uint32_t set_mask)
{
	uint32_t reg32 = pci_read_config32(dev, where);
	reg32 &= ~clr_mask;
	reg32 |= set_mask;
	pci_write_config32(dev, where, reg32);
}
