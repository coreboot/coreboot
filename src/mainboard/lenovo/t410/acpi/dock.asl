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
			   \_SB.PCI0.LPCB.EC.DKR2 = 1
			   \_SB.PCI0.LPCB.EC.DKR3 = 1
			} else {
			   /* disconnect dock */
			   \GP28 = 0
			   \_SB.PCI0.LPCB.EC.DKR1 = 0
			   \_SB.PCI0.LPCB.EC.DKR2 = 0
			   \_SB.PCI0.LPCB.EC.DKR3 = 0
			}
			Local0 = Arg0 ^ \_SB.PCI0.LPCB.EC.DKR1
			Return (Local0)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC.DKR1)
		}
	}
}

Scope(\_SB.PCI0.LPCB.EC)
{
	Method(_Q18, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 3)
	}

	Method(_Q45, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 3)
	}

	Method(_Q58, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 0)
	}

	Method(_Q37, 0, NotSerialized)
	{
		Notify(\_SB.DOCK, 0)
	}
}
