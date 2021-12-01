/* SPDX-License-Identifier: GPL-2.0-only */

Device (AC)
{
	Name (_HID, "ACPI0003")
	Name (_PCL, Package () { \_SB })

	Method (_PSR)
	{
		Return (\_SB.PCI0.LPCB.EC0.ADP)
	}
}
