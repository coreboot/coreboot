/* SPDX-License-Identifier: GPL-2.0-only */

#include <superio/nuvoton/npcd378/acpi/superio.asl>

Scope (\_GPE)
{
	Method (_L0D, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.EHC1, 0x02)
		Notify (\_SB.PCI0.EHC2, 0x02)
		//FIXME: Add GBE device
		//Notify (\_SB.PCI0.GBE, 0x02)
	}

	Method (_L09, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.RP01, 0x02)
		Notify (\_SB.PCI0.RP02, 0x02)
		Notify (\_SB.PCI0.RP03, 0x02)
		Notify (\_SB.PCI0.RP04, 0x02)
		Notify (\_SB.PCI0.RP05, 0x02)
		Notify (\_SB.PCI0.RP06, 0x02)
		Notify (\_SB.PCI0.RP07, 0x02)
		Notify (\_SB.PCI0.RP08, 0x02)
		Notify (\_SB.PCI0.PEGP, 0x02)
	}
}
