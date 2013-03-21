/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_ids.h>

/*
 * Enable 4MB (LPC) ROM access at 0xFFC00000 - 0xFFFFFFFF.
 *
 * Hardware should enable LPC ROM by pin straps. This function does not
 * handle the theoretically possible PCI ROM, FWH, or SPI ROM configurations.
 *
 * The HUDSON power-on default is to map 512K ROM space.
 *
 */
static void hudson_enable_rom(void)
{
	u8 reg8;
	device_t dev;

	dev = PCI_DEV(0, 0x14, 3);

	/* Decode variable LPC ROM address ranges 1 and 2. */
	reg8 = pci_read_config8(dev, 0x48);
	reg8 |= (1 << 3) | (1 << 4);
	pci_write_config8(dev, 0x48, reg8);

	/* LPC ROM address range 1: */
	/* Enable LPC ROM range mirroring start at 0x000e(0000). */
	pci_write_config16(dev, 0x68, 0x000e);
	/* Enable LPC ROM range mirroring end at 0x000f(ffff). */
	pci_write_config16(dev, 0x6a, 0x000f);

	/* LPC ROM address range 2: */
	/*
	 * Enable LPC ROM range start at:
	 * 0xfff8(0000): 512KB
	 * 0xfff0(0000): 1MB
	 * 0xffe0(0000): 2MB
	 * 0xffc0(0000): 4MB
	 */
	pci_write_config16(dev, 0x6c, 0x10000 - (CONFIG_COREBOOT_ROMSIZE_KB >> 6));
	/* Enable LPC ROM range end at 0xffff(ffff). */
	pci_write_config16(dev, 0x6e, 0xffff);
}

static void bootblock_southbridge_init(void)
{
	hudson_enable_rom();
}
