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
		Offset(EC_HOTPLUG_MODE),
		HPMO, 1,			/* EC_HOTPLUG_MODE_SLOT0 */
		Offset(EC_HOTPLUG_BASIC_PWR_EN),
		HPPW, 1,			/* EC_HP_BASIC_PWR_EN_SLOT0 */
		Offset(EC_HOTPLUG_BASIC_RESET_N),
		HPRN, 1,			/* EC_HP_BASIC_RESET_N_SLOT0 */
		Offset(EC_M2_POWER),
		MPWR, 1,
		, 3,
		MRST, 1,
		Offset(EC_PAGE_SELECT),
		PAGE, 8,
		Offset(EC_EVAL_STS),
		, 1,
		EVPG,	1,	/* EC_EVAL_STS_PWR_GOOD */
		EVCD,	1,	/* EC_EVAL_STS_CARD_DETECTED */
		Offset(EC_EVAL_CTRL),
		, 1,
		EVON, 1,	/* EC_EVAL_CTRL_CARD_ON */
		Offset(EC_SLOT_PWR_CTRL),
		, 5,
		STPW, 1,	/* EC_SLOT0_PWR_ENABLE */
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

// dGPU BOCO support on PCIe Slot 0
Scope (\_SB.PCI0.GP19.SWUS.SWDS.DGPU)
{
	// Hint OS about dependency to EC
	Name (_DEP, Package (0x01)
	{
		\_SB.PCI0.LPCB.EC0
	})

	/* Get BOCO enabled state */
	Method (BCEN, 0)
	{
		/* Advertise BOCO only when Eval card is present in Slot0 (EVCD).
		 *
		 * The Eval card has:
		 *  - Upstream switch port (SWUS)
		 *  - Downstream switch port (SWDS)
		 *  - The endpoint (DGPU) with onboard VR enable line (EVON)
		 */
		Return (\_SB.PCI0.LPCB.EC0.EVCD)
	}

	/* Get power state */
	Method (PWRS, 0)
	{
		If (\_SB.PCI0.LPCB.EC0.OKEC == 0)
		{
			Debug = "EC not ready"
			Return (0)
		}

		Return (\_SB.PCI0.LPCB.EC0.EVON)
	}

	/* Runs the mainboard specific BOCO power up/down sequence.
	 * Arg0 : 0 = Power Off, 1 = Power on
	 */
	Method (BOCO, 1, Serialized)
	{
		If (\_SB.PCI0.LPCB.EC0.OKEC == 0)
		{
			Debug = "EC not ready"
			Return ()
		}

		/*
		 * PCIe Slot0 is in enhanced hotplug.
		 * Hotplug controller will drive reset and power controlled by EC_SLOT0_PWR_ENABLE.
		 * FIXME: Due to firmware bug EC_SLOT0_PWR_ENABLE isn't working.
		 * WORKAROUND: Switch to Basic Hotplug mode and driving power and reset manually.
		 *             Increases power sequencing duration by several seconds, due to EC
		 *             GPIO PAGE slow access times.
		 */
		If (Arg0 == 0 && \_SB.PCI0.LPCB.EC0.EVON == 1) {
			\_SB.PCI0.LPCB.EC0.HPRN = 0	/* RESET_N_SLOT0 = 0 when in BasicHotplug Mode */
			\_SB.PCI0.LPCB.EC0.HPMO = 0	/* Set BasicHotplug Mode */
			\_SB.PCI0.LPCB.EC0.EVON = 0	/* Turn off Eval Card */
			\_SB.PCI0.LPCB.EC0.HPPW = 0	/* Turn off SLOT0 power when in BasicHotplug Mode */
			Sleep (10)
			Debug = "BOCO powered off"
		} ElseIf (Arg0 == 1 && \_SB.PCI0.LPCB.EC0.EVON == 0) {
			\_SB.PCI0.LPCB.EC0.HPRN = 0	/* RESET_N_SLOT0 = 0 when in BasicHotplug Mode */
			\_SB.PCI0.LPCB.EC0.HPMO = 0	/* Set BasicHotplug Mode */
			\_SB.PCI0.LPCB.EC0.HPPW = 1	/* Turn on SLOT0 power when in BasicHotplug Mode */
			Sleep (1)

			\_SB.PCI0.LPCB.EC0.EVON = 1	/* Turn on Eval Card */
			Local0 = 500

			/* Wait for EVAL power good */
			While (\_SB.PCI0.LPCB.EC0.EVPG == 0 && Local0 > 0) {
				Stall (100)
				Local0 --
			}
			Stall (100)

			\_SB.PCI0.LPCB.EC0.HPRN = 1	/* RESET_N_SLOT0 = 1 when in BasicHotplug Mode */
			\_SB.PCI0.LPCB.EC0.HPMO = 1	/* Set EnhancedHotplug Mode */
			Sleep (100)
			Debug = "BOCO powered on"
		} Else {
			Debug = "Invalid BOCO state"
		}

		Return ()
	}
}
