/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Framework EC over eSPI */
Scope (\_SB.PCI0.LPCB)
{
	#include <ec/google/chromeec/acpi/superio.asl>
	#include <ec/google/chromeec/acpi/ec.asl>
}

/*
 * Clear ACPI driver ready before entering suspend, same as the vendor
 * firmware, so the EC returns to preOS mode across the power transition.
 */
Method (\_SB.MPTS, 1, Serialized)
{
	If (Arg0) {
		\_SB.PCI0.LPCB.EC0.ADRD = 0
	}
}

/*
 * Signal ACPI driver ready to EC again, after resume from suspend,
 * same as on boot, to make sure function keys, etc. are working.
 */
Method (\_SB.MWAK, 1, Serialized)
{
	If ((Arg0 == 0x03) || (Arg0 == 0x04)) {
		\_SB.PCI0.LPCB.EC0.ADRD = 1
	}
}
