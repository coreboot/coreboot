/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

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
		1,            //  3: Battery Technology(Rechargable)
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
		While (LEqual (HSID, Zero))
		{
			// EC Is not ready
			Sleep (5)
			Decrement (Local0)
			If (LEqual (Local0, Zero))
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
		XOr (Local7, One, Index (PBIF, 0))

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
			Multiply (SBFC, 10, Index (PBIF, 2))
		}
		Else
		{
			Store (SBFC, Index (PBIF, 2))
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
			Multiply (SBDC, 10, Local0)
		}
		Else
		{
			Store (SBDC, Local0)
		}
		Store (Local0, Index(PBIF, One))

		//
		//  Design capacity of High (5%)
		//  Design capacity of Low (1%)
		//
		Divide (Local0,  20, Local1, Index (PBIF, 5))
		Divide (Local0, 100, Local1, Index (PBIF, 6))

		//
		//  Design voltage
		//
		Store (SBDV, Index (PBIF, 4))

		//
		// Serial Number
		//
		Store (ToHexString (SBSN), Index (PBIF, 10))

		//
		// Information ID 4 -
		//
		Store (4, HIID)
		WAEC ()

		//
		//  Battery Type - Device Chemistry
		//
		Store (ToString (SBCH), Index (PBIF, 11))

		//
		// Information ID 5 -
		//
		Store (5, HIID)
		WAEC ()

		//
		// OEM Information - Manufacturer Name
		//
		Store (ToString (SBMN), Index (PBIF, 12))

		//
		// Information ID 6 -
		//
		Store (6, HIID)
		WAEC ()

		//
		// Model Number - Device Name
		//
		Store (ToString (SBDN), Index (PBIF, 9))

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
		If (LEqual (And (HB0S, 0x0F), 0))
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
		Store (Local1, Index (PBST, 0))

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//
		Store (ECAC, Local1)
		If (LGreaterEqual (Local1, 0x8000))
		{
			If (And (Local0, 1))
			{
				Subtract (0x10000, Local1, Local1)
			}
			Else
			{
				// Error
				Store (Zero, Local1)
			}
		}
		Else
		{
			If (LNot (AND (Local0, 2)))
			{
				// Battery is not charging
				Store (Zero, Local1)
			}
		}

		XOr (DerefOf (Index (PBIF, Zero)), One, Local6)

		If (Local6)
		{
			Multiply (ECVO, Local1, Local1)
			Divide (Local1, 1000, Local7, Local1)
		}
		Store (Local1, Index (PBST, One))

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		// Get Power unit from the battery static information
		//   SMART battery : 1 - 10mWh : 0 - mAh
		//   ACPI spec     : 0 - mWh   : 1 - mAh
		If (Local6)
		{
			Multiply (ECRC, 10, Local1)
		}
		Else
		{
			Store (ECRC, Local1)
		}

		If (LAnd (BFWK, LAnd (ACPW, LNot (Local0))))
		{
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			// TODO: Is SBRS the "battery gas gauge"?
			Store (SBRS, Local2)

			// See if within ~3% of full
			ShiftRight (Local2, 5, Local3)
			If (LAnd (LGreater (Local1, Subtract (Local2, Local3)),
			          LLess (Local1, Add (Local2, Local3))))
			{
				Store (Local2, Local1)
			}
		}
		Store (Local1, Index (PBST, 2))

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		Store (ECVO, Index (PBST, 3))

		Return (PBST)
	}
}
