/* SPDX-License-Identifier: GPL-2.0-only */

Field(ERAM, ByteAcc, NoLock, Preserve)
{
		Offset (0x46),
				, 4,
			    HPAC, 1
}

Device(AC)
{
	Name(_HID, "ACPI0003")
	Name(_UID, 0x00)
	Name(_PCL, Package() { \_SB } )

	Method(_PSR, 0, NotSerialized)
	{
		Local0 = HPAC
		\PWRS = Local0
		\PNOT()
		return (Local0)
	}

	Method(_STA, 0, NotSerialized)
	{
		Return (0x0f)
	}
}
