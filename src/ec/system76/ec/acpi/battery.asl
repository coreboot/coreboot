/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A") /* Control Method Battery */)  // _HID: Hardware ID
	Name (_UID, Zero)  // _UID: Unique ID
	Name (_PCL, Package (0x01)  // _PCL: Power Consumer List
	{
		_SB
	})
	Name (BFCC, Zero)
	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		If (^^PCI0.LPCB.EC0.ECOK)
		{
			If (^^PCI0.LPCB.EC0.BAT0)
			{
				Return (0x1F)
			}
			Else
			{
				Return (0x0F)
			}
		}
		Else
		{
			Return (0x0F)
		}
	}

	Name (PBIF, Package (0x0D)
	{
		1, // 0 - Power Unit
		0xFFFFFFFF, // 1 - Design Capacity
		0xFFFFFFFF, // 2 - Last Full Charge Capacity
		1, // 3 - Battery Technology
		0xFFFFFFFF, // 4 - Design Voltage
		Zero, // 5 - Design Capacity of Warning
		Zero, // 6 - Design Capacity of Low
		0x40, // 7 - Battery Capacity Granularity 1
		0x40, // 8 - Battery Capacity Granularity 2
		" ", // 9 - Model Number
		" ", // 10 - Serial Number
		" ", // 11 - Battery Type
		" " // 12 - OEM Information
	})
	Method (IVBI, 0, NotSerialized)
	{
		PBIF [1] = 0xFFFFFFFF
		PBIF [2] = 0xFFFFFFFF
		PBIF [4] = 0xFFFFFFFF
		PBIF [9] = " "
		PBIF [10] = " "
		PBIF [11] = " "
		PBIF [12] = " "
		BFCC = Zero
	}

	Method (UPBI, 0, NotSerialized)
	{
		If (^^PCI0.LPCB.EC0.BAT0)
		{
			Local0 = (^^PCI0.LPCB.EC0.BDC0 & 0xFFFF)
			PBIF [1] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BFC0 & 0xFFFF)
			PBIF [2] = Local0
			BFCC = Local0
			Local0 = (^^PCI0.LPCB.EC0.BDV0 & 0xFFFF)
			PBIF [4] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BCW0 & 0xFFFF)
			PBIF [5] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BCL0 & 0xFFFF)
			PBIF [6] = Local0
			PBIF [9] = "BAT"
			PBIF [10] = "0001"
			PBIF [11] = "LION"
			PBIF [12] = "Notebook"
		}
		Else
		{
			IVBI ()
		}
	}

	Method (_BIF, 0, NotSerialized)  // _BIF: Battery Information
	{
		If (^^PCI0.LPCB.EC0.ECOK)
		{
			UPBI ()
		}
		Else
		{
			IVBI ()
		}

		Return (PBIF) /* \_SB_.BAT0.PBIF */
	}

	Name (PBIX, Package ()
	{
		0, // 0 - Revision
		1, // 1 - Power Unit: mAh
		0xFFFFFFFF, // 2 - Design Capacity
		0xFFFFFFFF, // 3 - Last Full Charge Capacity
		1, // 4 - Battery Technology: Rechargeable
		0xFFFFFFFF, // 5 - Design Voltage
		0, // 6 - Design Capacity of Warning
		0, // 7 - Design Capacity of Low
		0, // 8 - Cycle Count
		98000, // 9 - Measurement Accuracy
		0xFFFFFFFF, // 10 - Max Sampling Time
		0xFFFFFFFF, // 11 - Min Sampling Time
		0xFFFFFFFF, // 12 - Max Averaging Interval
		0xFFFFFFFF, // 13 - Min Averaging Interval
		0x40, // 14 - Battery Capacity Granularity 1
		0x40, // 15 - Battery Capacity Granularity 2
		" ", // 16 - Model Number
		" ", // 17 - Serial Number
		" ", // 18 - Battery Type
		" " // 19 - OEM Information
	})

	Method (IVBX, 0, NotSerialized)
	{
		PBIX [2] = 0xFFFFFFFF
		PBIX [3] = 0xFFFFFFFF
		PBIX [5] = 0xFFFFFFFF
		PBIX [16] = " "
		PBIX [17] = " "
		PBIX [18] = " "
		PBIX [19] = " "
		BFCC = 0
	}

	Method (UPBX, 0, NotSerialized)
	{
		If (^^PCI0.LPCB.EC0.BAT0)
		{
			Local0 = (^^PCI0.LPCB.EC0.BDC0 & 0xFFFF)
			PBIX [2] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BFC0 & 0xFFFF)
			PBIX [3] = Local0
			BFCC = Local0
			Local0 = (^^PCI0.LPCB.EC0.BDV0 & 0xFFFF)
			PBIX [5] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BCW0 & 0xFFFF)
			PBIX [6] = Local0
			Local0 = (^^PCI0.LPCB.EC0.BCL0 & 0xFFFF)
			PBIX [7] = Local0
			LOCAL0 = ^^PCI0.LPCB.EC0.CYC0
			PBIX [8] = LOCAL0
			PBIX [16] = "BAT"
			PBIX [17] = "0001"
			PBIX [18] = "LION"
			PBIX [19] = "Notebook"
		}
		Else
		{
			IVBX ()
		}
	}

	// _BIX: Battery Information Extended
	Method (_BIX, 0, NotSerialized)
	{
		If (^^PCI0.LPCB.EC0.ECOK)
		{
			UPBX ()
		}
		Else
		{
			IVBX ()
		}
		Return (PBIX) /* \_SB_.BAT0.PBIX */
	}

	Name (PBST, Package (0x04)
	{
		Zero, // 0 - Battery state
		0xFFFFFFFF, // 1 - Battery present rate
		0xFFFFFFFF, // 2 - Battery remaining capacity
		0xFFFFFFFF // 3 - Battery present voltage
	})
	Method (IVBS, 0, NotSerialized)
	{
		PBST [0] = Zero
		PBST [1] = 0xFFFFFFFF
		PBST [2] = 0xFFFFFFFF
		PBST [3] = 0xFFFFFFFF
	}

	Method (UPBS, 0, NotSerialized)
	{
		If (^^PCI0.LPCB.EC0.BAT0)
		{
			Local0 = Zero
			Local1 = Zero
			If (^^AC.ACFG)
			{
				If (((^^PCI0.LPCB.EC0.BST0 & 0x02) == 0x02))
				{
					Local0 |= 0x02
					Local1 = (^^PCI0.LPCB.EC0.BPR0 & 0xFFFF)
				}
			}
			Else
			{
				Local0 |= 1
				Local1 = (^^PCI0.LPCB.EC0.BPR0 & 0xFFFF)
			}

			Local7 = (Local1 & 0x8000)
			If ((Local7 == 0x8000))
			{
				Local1 ^= 0xFFFF
			}

			Local2 = (^^PCI0.LPCB.EC0.BRC0 & 0xFFFF)
			Local3 = (^^PCI0.LPCB.EC0.BPV0 & 0xFFFF)
			PBST [0] = Local0
			PBST [1] = Local1
			PBST [2] = Local2
			PBST [3] = Local3
			If ((BFCC != ^^PCI0.LPCB.EC0.BFC0))
			{
				Notify (BAT0, 0x81) // Information Change
			}
		}
		Else
		{
			IVBS ()
		}
	}

	Method (_BST, 0, NotSerialized)  // _BST: Battery Status
	{
		If (^^PCI0.LPCB.EC0.ECOK)
		{
			UPBS ()
		}
		Else
		{
			IVBS ()
		}

		Return (PBST) /* \_SB_.BAT0.PBST */
	}
}
