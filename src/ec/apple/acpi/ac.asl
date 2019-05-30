/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_SB.PCI0.LPCB.EC)
{
	Field(ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(HPAC_OFFSET),
			, 1,
		HPAC, 1,  /* AC status */
	}

	Device(AC)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )

		Method(_PSR, 0, NotSerialized)
		{
			Return(HPAC)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return(0x0f)
		}
	}
}
