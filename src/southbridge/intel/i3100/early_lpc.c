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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

static void i3100_enable_superio(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Enable decoding of I/O locations for SuperIO devices */
	pci_write_config16(dev, 0x80, 0x0010);
	pci_write_config16(dev, 0x82, 0x340f);

	/* Enable the SERIRQs (start pulse width is 8 clock cycles) */
	pci_write_config8(dev, 0x64, 0xD2);
}

static void i3100_halt_tco_timer(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Temporarily enable the ACPI I/O range at 0x4000 */
	pci_write_config32(dev, 0x40, 0x4000 | (1 << 0));
	pci_write_config32(dev, 0x44, pci_read_config32(dev, 0x44) | (1 << 7));

	/* Halt the TCO timer, preventing SMI and automatic reboot */
	outw(inw(0x4068) | (1 << 11), 0x4068);

	/* Disable the ACPI I/O range */
	pci_write_config32(dev, 0x44, pci_read_config32(dev, 0x44) & ~(1 << 7));
}
