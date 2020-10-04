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
			   Sleep(250)
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
	OperationRegion(PMH7, SystemIO, 0x15e0, 0x10)
	Field(PMH7, ByteAcc, NoLock, Preserve)
	{
		Offset(0x0c),
			PIDX, 8,
		Offset(0x0e),
			PDAT, 8,
	}

	IndexField(PIDX, PDAT, ByteAcc, NoLock, Preserve)
	{
		Offset (0x61),
			DPWR, 1,
	}

	Method(_Q18, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 3)
	}

	Method(_Q37, 0, NotSerialized)
	{
		if (DPWR) {
			Notify(\_SB.DOCK, 0)
		} else {
			Notify(\_SB.DOCK, 3)
		}
	}

	Method(_Q50, 0, NotSerialized)
	{
		if (\_SB.DOCK._STA()) {
			Notify(\_SB.DOCK, 1)
		}
	}
}
