/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <reset.h>

static unsigned get_sbdn(unsigned bus)
{
	device_t dev;

	/* Find the device. */
	dev = pci_locate_device_on_bus(
		PCI_ID(PCI_VENDOR_ID_SIS, PCI_DEVICE_ID_SIS_SIS761),
		bus);

	return (dev>>15) & 0x1f;
}

void hard_reset(void)
{
	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

static void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	/* default value for sis966 is good */
	/* set VFSMAF ( VID/FID System Management Action Field) to 2 */
}

void soft_reset(void)
{
	set_bios_reset();

	/* link reset */
	outb(0x02, 0x0cf9);
	outb(0x06, 0x0cf9);

}

