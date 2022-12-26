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
	Name (BXST, 0)

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
	Name (BFWK, 1)

	// Method to enable full battery workaround
	Method (BFWE)
	{
		BFWK = 1
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		BFWK = 0
	}

	// Method to wait for EC to be ready after changing the Battery Info ID
	// Selector
	Method (WAEC)
	{
		Local0 = 20	// Timeout 100 msec
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
		BXST = MBTS
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
		HIID = 1
		WAEC ()

		//
		//  Power Unit
		//   SMART battery : 1 - 10mWh : 0 - mAh
		//   ACPI spec     : 0 - mWh   : 1 - mAh
		//
		Local7 = SBCM
		PBIF[0] = Local7 ^ 1

		//
		// Information ID 0 -
		//
		HIID = 0
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
			PBIF[2] = SBFC
		}

		//
		// Information ID 2 -
		//
		HIID = 2
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
			Local0 = SBDC
		}
		PBIF[1] = Local0

		//
		//  Design capacity of High (5%)
		//  Design capacity of Low (1%)
		//
		PBIF[5] = Local0 / 20
		PBIF[6] = Local0 / 100

		//
		//  Design voltage
		//
		PBIF[4] = SBDV

		//
		// Serial Number
		//
		PBIF[10] = ToHexString (SBSN)

		//
		// Information ID 4 -
		//
		HIID = 4
		WAEC ()

		//
		//  Battery Type - Device Chemistry
		//
		PBIF[11] = ToString (Concatenate(SBCH, 0x00))

		//
		// Information ID 5 -
		//
		HIID = 5
		WAEC ()

		//
		// OEM Information - Manufacturer Name
		//
		PBIF[12] = ToString (Concatenate(SBMN, 0x00))

		//
		// Information ID 6 -
		//
		HIID = 6
		WAEC ()

		//
		// Model Number - Device Name
		//
		PBIF[9] = ToString (Concatenate(SBDN, 0x00))

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
		If (HB0S & 0x20)
		{
			Local0 = 2
		}
		Else
		{
			if (HB0S & 0x40)
			{
				Local0 = 1
			}
			Else
			{
				Local0 = 0
			}
		}

		// Set critical flag if battery is empty
		If (HB0S & 0x0F == 0)
		{
			Local0 |= 4
		}

		Local1 = 0

		// Check if AC is present
		If (ACPW)
		{
			// Set only charging/discharging bits
			Local1 = Local0 & 3
		}
		Else
		{
			// Always discharging when on battery power
			Local1 = 1
		}

		// Flag if the battery level is critical
		Local4 = Local0 & 4
		Local1 |= Local4
		PBST[0] = Local1

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//
		Local1 = ECAC
		If (Local1 >= 0x8000)
		{
			If (Local0 & 1)
			{
				Local1 = 0x10000 - Local1
			}
			Else
			{
				// Error
				Local1 = 0
			}
		}
		Else
		{
			If (!(Local0 & 2))
			{
				// Battery is not charging
				Local1 = 0
			}
		}

		Local6 = DerefOf (PBIF[0]) ^ 1

		If (Local6)
		{
			Local1 *= ECVO
			Local1 /= 1000
		}
		PBST[1] = Local1

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
			Local1 = ECRC
		}

		If (BFWK && ACPW && !Local0)
		{
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			// TODO: Is SBRS the "battery gas gauge"?
			Local2 = SBRS

			// See if within ~3% of full
			Local3 = Local2 >> 5
			If (Local1 > Local2 - Local3 && Local1 < Local2 + Local3)
			{
				Local1 = Local2
			}
		}
		PBST[2] = Local1

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		PBST[3] = ECVO

		Return (PBST)
	}
}
