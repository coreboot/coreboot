/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

static void i3100_enable_superio(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_INTEL,
				       PCI_DEVICE_ID_INTEL_3100_LPC), 0);
	if (dev == PCI_DEV_INVALID) {
		die("LPC bridge not found\r\n");
	}

	/* Enable decoding of I/O locations for SuperIO devices */
	pci_write_config16(dev, 0x82, 0x340f);
}
