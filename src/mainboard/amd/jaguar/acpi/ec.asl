/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "ec.h"

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))  // ACPI Embedded Controller
	Name (_UID, 0)
	Name (_GPE, 10)

	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, JAGUAR_EC_DATA, JAGUAR_EC_DATA, 1, 1)
		IO (Decode16, JAGUAR_EC_CMD, JAGUAR_EC_CMD, 1, 1)
	})

	// Access once OKEC returns 1
	OperationRegion (RAM, EmbeddedControl, 0, 0xFF)
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		Offset(EC_M2_POWER),
		MPWR, 1,
		, 3,
		MRST, 1,
		Offset(EC_PAGE_SELECT),
		PAGE, 8,
	}

	// OKEC returns 1 once the EmbeddedControl OS driver has been loaded
	Name(OKEC, Zero)

	// OS runs _REG control on change in the availability of OpRegion
	Method (_REG, 2)
	{
		If (Arg0 == 3) // EmbeddedControl
		{
			OKEC = Arg1  // 1: Loaded, 0: Unloaded
		}
	}
}

Scope (\_SB.PCI0.GP11)
{
	// Hint OS about dependency to EC
	Name (_DEP, Package (0x01)
	{
		\_SB.PCI0.LPCB.EC0
	})

	Name (_PR0, Package (0x01)
	{
		PR00
	})
	Name (_PR3, Package (0x01)
	{
		PR00
	})
	PowerResource (PR00, 0, 0)
	{
		Method (_STA, 0, NotSerialized)
		{
			// Opt out when EC not ready yet
			If (\_SB.PCI0.LPCB.EC0.OKEC == 0)
			{
				Debug = "_STA returns 0"
				Return (Zero)
			}

			\_SB.PCI0.LPCB.EC0.PAGE = EC_GPIO_PAGE
			Local0 = \_SB.PCI0.LPCB.EC0.MPWR
			Local1 = \_SB.PCI0.LPCB.EC0.MRST
			Local0 &= Local1
			If (Local0)
			{
				Debug = "_STA returns 1"
			} Else {
				Debug = "_STA returns 0"
			}
			Return (Local0)
		}

		Method (_ON, 0, NotSerialized)
		{
			// Opt out when EC not ready yet
			If (\_SB.PCI0.LPCB.EC0.OKEC == 0)
			{
				Return ()
			}
			If (_STA () == One)
			{
				Return ()
			}

			\_SB.PCI0.LPCB.EC0.PAGE = EC_GPIO_PAGE

			\_SB.PCI0.LPCB.EC0.MPWR = One
			Sleep (10)
			\_SB.PCI0.LPCB.EC0.MRST = One
			Sleep (100)
			Debug = "_ON method called"
		}

		Method (_OFF, 0, NotSerialized)
		{
			// Opt out when EC not ready yet
			If (\_SB.PCI0.LPCB.EC0.OKEC == 0)
			{
				Return ()
			}
			If (_STA () == Zero)
			{
				Return ()
			}

			\_SB.PCI0.LPCB.EC0.PAGE = EC_GPIO_PAGE

			\_SB.PCI0.LPCB.EC0.MRST = Zero
			\_SB.PCI0.LPCB.EC0.MPWR = Zero
			Sleep (10)
			Debug = "_OFF method called"
		}
	}
}
