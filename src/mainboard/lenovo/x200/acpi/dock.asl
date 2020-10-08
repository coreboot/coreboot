/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	Device(DOCK)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )

		Method(_DCK, 1, NotSerialized)
		{
			if (Arg0) {
			   /* connect dock */
			   \GP28 = 1
			   \_SB.PCI0.LPCB.EC.DKR1 = 1
			} else {
			   /* disconnect dock */
			   \GP28 = 0
			   \_SB.PCI0.LPCB.EC.DKR1 = 0
			}
			Local0 = Arg0 ^ \_SB.PCI0.LPCB.EC.DKR1
			Return (Local0)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC.DKR1)
		}

		Name (G_ID, 0xFFFFFFFF)
		/* Returns 0x7 (dock absent) or 0x3 (dock present) */
		Method(GGID, 0, NotSerialized)
		{
			Local0 = G_ID
			if (Local0 == 0xFFFFFFFF)
			{
				Local0 = GP02 | (GP03 << 1) | (GP04 << 2)
				If (Local0 == 0x00)
				{
					Local0 = 0x03
				}
				G_ID = Local0
			}
			return (Local0)
		}
	}
}

Scope(\_SB.PCI0.LPCB.EC)
{
	Method(_Q18, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 3)
	}

	Method(_Q37, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 0)
	}

	Method(_Q45, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 3)
	}

	/* Undock button on dock */
	Method(_Q50, 0, NotSerialized)
	{
		Local0 = \_SB.DOCK.GGID ()
		if (Local0 != 0x07)
		{
			Notify(\_SB.DOCK, 3)
		}
	}

	Method(_Q58, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 0)
	}

	/* Unplug power: only disconnect dock on force eject */
	Method(_Q5A, 0, NotSerialized)
	{
		Local0 = \_SB.DOCK.GGID ()
		if (Local0 == 0x07)
		{
			Notify(\_SB.DOCK, 3)
		}
		if (Local0 == 0x03)
		{
			Sleep(0x64)
			Local1 = DKR1
			if (Local1 == 1)
			{
				Notify(\_SB.DOCK, 0)
			}
		}
	}
}
