/* SPDX-License-Identifier: GPL-2.0-only */

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
	// If PICM is _not_ set, interrupts are routed over the i8259, otherwise
	// over the IOAPIC. (Really? If they're above 15 they need to be routed
	// fixed over the IOAPIC?)

	Method (_PRT)
	{
		#include "acpi/ich10_pci_irqs.asl"
	}

}
