/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
			   Store (1, \GP28)
			   Store (1, \_SB.PCI0.LPCB.EC.DKR1)
			} else {
			   /* disconnect dock */
			   Store (0, \GP28)
			   Store (0, \_SB.PCI0.LPCB.EC.DKR1)
			}
			Xor(Arg0, \_SB.PCI0.LPCB.EC.DKR1, Local0)
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
			Store(G_ID, Local0)
			if (LEqual(Local0, 0xFFFFFFFF))
			{
				Store(Or (Or (GP02, ShiftLeft(GP03, 1)),
					 ShiftLeft(GP04, 2)), Local0)
				If (LEqual(Local0, 0x00))
				{
					Store(0x03, Local0)
				}
				Store(Local0, G_ID)
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
		Store(\_SB.DOCK.GGID (), Local0)
		if (LNotEqual(Local0, 0x07))
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
		Store(\_SB.DOCK.GGID (), Local0)
		if (LEqual(Local0, 0x07))
		{
			Notify(\_SB.DOCK, 3)
		}
		if (LEqual(Local0, 0x03))
		{
			Sleep(0x64)
			Store(DKR1, Local1)
			if (LEqual(Local1, 1))
			{
				Notify(\_SB.DOCK, 0)
			}
		}
	}
}
