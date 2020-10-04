/* SPDX-License-Identifier: GPL-2.0-only */

#include "smi.h"

Scope (\_SB)
{
	OperationRegion (DLPC, SystemIO, 0x164c, 1)
	Field(DLPC, ByteAcc, NoLock, Preserve)
	{
		    ,	3,
		DSTA,	1,
	}

	Device(DOCK)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )

		Method(_DCK, 1, NotSerialized)
		{
			if (Arg0) {
			   /* connect dock */
			   TRAP(SMI_DOCK_CONNECT)
			} else {
			   /* disconnect dock */
			   TRAP(SMI_DOCK_DISCONNECT)
			}

			Local0 = Arg0 ^ DSTA
			Return (Local0)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return (DSTA)
		}
	}
}

Scope(\_SB.PCI0.LPCB.EC)
{
	Method(_Q18, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 3)
	}

	Method(_Q50, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 3)
	}

	Method(_Q58, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 0)
	}

}
