/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

#define PCI_HOST_BRIDGE_OSC_UUID "33db4d5b-1ff7-401c-9657-7441c03dd766"

Scope (\_SB)
{
	Method (DOSC, 4)
	{
		If (Arg0 != ToUUID (PCI_HOST_BRIDGE_OSC_UUID))
		{
			CreateDWordField (Arg3, 0, DW1)
			DW1 |= 4 // Unrecognized UUID
		}
		Return (Arg3)
	}

	Device (PCI0)
	{
		Method (_STA)
		{
			Return (0xf)
		}
		Name (_HID, EISAID ("PNP0A08")) // PCI Express Bus
		Name (_CID, EISAID ("PNP0A03")) // PCI Bus
		Name (_UID, "PCI0")
		Name (_PXM, 0)
		Method (_OSC, 4)
		{
			Return (\_SB.DOSC (Arg0, Arg1, Arg2, Arg3))
		}
	}
}
