/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@settoplinux.org>
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

static void i82801dx_halt_tco_timer(void)
{
	/* Set the LPC device statically. */
	device_t dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Temporarily set ACPI base address (I/O space). */
	pci_write_config32(dev, PMBASE, (PMBASE_ADDR | 1));

	/* Enable ACPI I/O. */
	pci_write_config8(dev, ACPI_CNTL, 0x10);

	/* Halt the TCO timer, preventing SMI and automatic reboot */
	outw(inw(PMBASE_ADDR + TCOBASE + TCO1_CNT) | (1 << 11),
	     PMBASE_ADDR + TCOBASE + TCO1_CNT);
}
