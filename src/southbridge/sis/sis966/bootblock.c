/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
#include <arch/io.h>
#include <device/pci_ids.h>
#include "sis966.h"

static void sis966_enable_rom(void)
{
	device_t addr;

	/* Enable 4MB ROM access at 0xFFC00000 - 0xFFFFFFFF. */
	addr = pci_locate_device(PCI_ID(PCI_VENDOR_ID_SIS,
					PCI_DEVICE_ID_SIS_SIS966_LPC), 0);

	/* Set the 4MB enable bit(s). */
	pci_write_config8(addr, 0x40, pci_read_config8(addr, 0x40) | 0x11);
}

static void bootblock_southbridge_init(void)
{
	sis966_enable_rom();
}
