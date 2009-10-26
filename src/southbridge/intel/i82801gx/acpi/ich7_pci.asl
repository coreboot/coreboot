/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

// Intel PCI to PCI bridge 0:1e.0

Device (PCIB)
{
	Name (_ADR, 0x001e0000)

	Device (SLT1)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (SLT2)
	{
		Name (_ADR, 0x00010000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (SLT3)
	{
		Name (_ADR, 0x00020000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (SLT6)
	{
		Name (_ADR, 0x00050000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (LANC)
	{
		Name (_ADR, 0x00080000)
		Name (_PRW, Package(){ 11, 3 })
	}

	Device (LANR)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package(){ 11, 3 })
	}

	// TODO: How many slots, where?

	// PCI Interrupt Routing.
	// If PICM is set, interrupts are routed over the i8259, otherwise
	// over the IOAPIC. (Really? If they're above 15 they need to be routed
	// fixed over the IOAPIC?)

	Method (_PRT)
	{
		#include "acpi/ich7_pci_irqs.asl"
	}

}

