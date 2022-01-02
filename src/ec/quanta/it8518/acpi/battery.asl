/* SPDX-License-Identifier: GPL-2.0-only */

// Scope (EC0)

Device (BATX)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	//
	// Indicator of BATX attach/detach
	// Battery X Information
	// Battery X Status
	//
	Name (BXST, Zero)

	//
	// Default Static Battery Information
	//
	Name (PBIF, Package()
	{
		0,            //  0: Power Unit
		0xFFFFFFFF,   //  1: Design Capacity
		0xFFFFFFFF,   //  2: Last Full Charge Capacity
		1,            //  3: Battery Technology(Rechargeable)
		10800,        //  4: Design Voltage 10.8V
		0,            //  5: Design capacity of warning
		0,            //  6: Design capacity of low
		1,            //  7: Battery capacity granularity 1
		1,            //  8: Battery capacity granularity 2
		"",           //  9: Model Number
		"",           // 10: Serial Number
		"",           // 11: Battery Type
		""            // 12: OEM Infomration
	})

	Name (PBST, Package ()
	{
		0x00000000,  // Battery State
		0xFFFFFFFF,  // Battery Present Rate
		0xFFFFFFFF,  // Battery Remaining Capacity
		0xFFFFFFFF,  // Battery Present Voltage
	})

	// Workaround for full battery status, enabled by default
	Name (BFWK, One)

	// Method to enable full battery workaround
	Method (BFWE)
	{
		Store (One, BFWK)
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		Store (Zero, BFWK)
	}

	// Method to wait for EC to be ready after changing the Battery Info ID
	// Selector
	Method (WAEC)
	{
		Store (20, Local0)	// Timeout 100 msec
		While (HSID == 0)
		{
			// EC Is not ready
			Sleep (5)
			Local0--
			If (Local0 == 0)
			{
				Break
			}
		}
	}

	// Battery Slot Status
	Method (_STA, 0, Serialized)
	{
		Store (MBTS, BXST)
		If (BXST)
		{
			// Battery is present
			Return (0x1F)
		}
		Else
		{
			Return (0x0F)
		}
	}

	Method (_BIF, 0, Serialized)
	{
		// Update fields from EC

		//
		// Information ID 1 -
		//
		Store (One, HIID)
		WAEC ()

		//
		//  Power Unit
		//   SMART battery : 1 - 10mWh : 0 - mAh
		//   ACPI spec     : 0 - mWh   : 1 - mAh
		//
		Store(SBCM, Local7)
		XOr (Local7, One, PBIF[0])

		//
		// Information ID 0 -
		//
		Store (Zero, HIID)
		WAEC ()

		//
		//  Last Full Charge Capacity
		//
		If (Local7)
		{
			PBIF[2] = SBFC * 10
		}
		Else
		{
			Store (SBFC, PBIF[2])
		}

		//
		// Information ID 2 -
		//
		Store (2, HIID)
		WAEC ()

		//
		//  Design capacity
		//
		If (Local7)
		{
			Local0 = SBDC * 10
		}
		Else
		{
			Store (SBDC, Local0)
		}
		Store (Local0, PBIF[1])

		//
		//  Design capacity of High (5%)
		//  Design capacity of Low (1%)
		//
		PBIF[5] = Local0 / 20
		PBIF[6] = Local0 / 100

		//
		//  Design voltage
		//
		Store (SBDV, PBIF[4])

		//
		// Serial Number
		//
		Store (ToHexString (SBSN), PBIF[10])

		//
		// Information ID 4 -
		//
		Store (4, HIID)
		WAEC ()

		//
		//  Battery Type - Device Chemistry
		//
		Store (ToString (Concatenate(SBCH, 0x00)), PBIF[11])

		//
		// Information ID 5 -
		//
		Store (5, HIID)
		WAEC ()

		//
		// OEM Information - Manufacturer Name
		//
		Store (ToString (Concatenate(SBMN, 0x00)), PBIF[12])

		//
		// Information ID 6 -
		//
		Store (6, HIID)
		WAEC ()

		//
		// Model Number - Device Name
		//
		Store (ToString (Concatenate(SBDN, 0x00)), PBIF[9])

		Return (PBIF)
	}

	Method (_BST, 0, Serialized)
	{
		// Update Battery First Used Date, if requested
		If (BFUD)
		{
			// TODO: Handle First Used Date Request
			//\BFUD()
		}

		//
		// 0: BATTERY STATE
		//
		// bit 0 = discharging
		// bit 1 = charging
		// bit 2 = critical level
		//

		// Get battery state from EC
		If (And (HB0S, 0x20))
		{
			Store (2, Local0)
		}
		Else
		{
			if (And (HB0S, 0x40))
			{
				Store (One, Local0)
			}
			Else
			{
				Store (Zero, Local0)
			}
		}

		// Set critical flag if battery is empty
		If (And (HB0S, 0x0F) == 0)
		{
			Or (Local0, 4, Local0)
		}

		Store (Zero, Local1)

		// Check if AC is present
		If (ACPW)
		{
			// Set only charging/discharging bits
			And (Local0, 0x03, Local1)
		}
		Else
		{
			// Always discharging when on battery power
			Store (One, Local1)
		}

		// Flag if the battery level is critical
		And (Local0, 0x04, Local4)
		Or (Local1, Local4, Local1)
		Store (Local1, PBST[0])

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//
		Store (ECAC, Local1)
		If (Local1 >= 0x8000)
		{
			If (And (Local0, 1))
			{
				Local1 = 0x10000 - Local1
			}
			Else
			{
				// Error
				Store (Zero, Local1)
			}
		}
		Else
		{
			If (!(AND (Local0, 2)))
			{
				// Battery is not charging
				Store (Zero, Local1)
			}
		}

		XOr (DerefOf (PBIF[0]), One, Local6)

		If (Local6)
		{
			Local1 *= ECVO
			Local1 /= 1000
		}
		Store (Local1, PBST[1])

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		// Get Power unit from the battery static information
		//   SMART battery : 1 - 10mWh : 0 - mAh
		//   ACPI spec     : 0 - mWh   : 1 - mAh
		If (Local6)
		{
			Local1 = ECRC * 10
		}
		Else
		{
			Store (ECRC, Local1)
		}

		If (BFWK && ACPW && !Local0)
		{
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			// TODO: Is SBRS the "battery gas gauge"?
			Store (SBRS, Local2)

			// See if within ~3% of full
			ShiftRight (Local2, 5, Local3)
			If (Local1 > Local2 - Local3 && Local1 < Local2 + Local3)
			{
				Store (Local2, Local1)
			}
		}
		Store (Local1, PBST[2])

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		Store (ECVO, PBST[3])

		Return (PBST)
	}
}
