/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#if !defined (__ROMCC__) && !defined (__PRE_RAM__)
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))
#define PCI_DEV_INVALID 0

static inline device_t pci_locate_device(unsigned pci_id, device_t from)
{
	return dev_find_device(pci_id >> 16, pci_id & 0xffff, from);
}
#endif

void soft_reset(void)
{
	outb(0x04, 0xcf9);
}

void hard_reset(void)
{
	outb(0x06, 0xcf9);
}

void full_reset(void)
{
	device_t dev;
	/* Enable power on after power fail... */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3100_LPC), 0);
	if (dev != PCI_DEV_INVALID) {
		unsigned byte;
		byte = pci_read_config8(dev, 0xa4);
		byte &= 0xfe;
		pci_write_config8(dev, 0xa4, byte);
	}
	outb(0x0e, 0xcf9);
}

