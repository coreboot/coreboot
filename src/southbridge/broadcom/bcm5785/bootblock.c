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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_ids.h>

/* Enable 4MB ROM access at 0xFFC00000 - 0xFFFFFFFF. */
static void bcm5785_enable_rom(void)
{
	u8 byte;
	device_t dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_SERVERWORKS,
			PCI_DEVICE_ID_SERVERWORKS_BCM5785_SB_PCI_MAIN), 0);

	/* Set the 4MB enable bits. */
	byte = pci_read_config8(dev, 0x41);
	byte |= 0x0e;
	pci_write_config8(dev, 0x41, byte);
}

static void bootblock_southbridge_init(void)
{
	bcm5785_enable_rom();
}
