/* SPDX-License-Identifier: GPL-2.0-only */

Device(AC)
{
	Name(_HID, "ACPI0003")
	Name(_UID, 0x00)
	Name(_PCL, Package() { \_SB } )

	Method(_PSR, 0, NotSerialized)
	{
		return (HPAC)
	}

	Method(_STA, 0, NotSerialized)
	{
		Return (0x0f)
	}
}

/* AC status change */
Method(_Q50, 0, NotSerialized)
{
	Notify (AC, 0x80)
}

/* AC status change */
Method(_Q51, 0, NotSerialized)
{
	Notify (AC, 0x80)
}
