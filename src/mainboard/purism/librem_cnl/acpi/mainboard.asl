/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.LPCB)
{
	Device (AC)
	{
		Name (_HID, "ACPI0003")
		Name (_PCL, Package () { LPCB })
		Name (ACFG, One)

		Method (_PSR, 0, NotSerialized)
		{
			Return (ACFG)
		}
	}
}
