/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

//SCOPE EC0

Device (BAT0)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })


	//
	// Default Static Battery Information
	//
	Name (PBIF, Package (13)
	{
		1,		//  0: Power Unit
		4800,		//  1: Design Capacity
		4650,		//  2: Last Full Charge Capacity
		1,		//  3: Battery Technology(Rechargeable)
		12608,		//  4: Design Voltage 10.8V
		0,		//  5: Design capacity of warning
		0,		//  6: Design capacity of low
		0,		//  7: Battery capacity granularity 1
		0,		//  8: Battery capacity granularity 2
		"BAT0",		//  9: Model Number
		"RT672",	// 10: Serial Number
		"LiON",		// 11: Battery Type
		"ASP"		// 12: OEM Information
	})


	Name (PBST, Package (4)
	{
		0x00000000,	// Battery State
		0xFFFFFFFF,	// Battery Present Rate
		0xFFFFFFFF,	// Battery Remaining Capacity
		0xFFFFFFFF,	// Battery Present Voltage
	})


	// Battery Slot Status
	Method (_STA, 0, Serialized)
	{
		Store ("-----> BAT0: _STA", Debug)

		Store (0x0F, Local0)

		Store (ECPS, Local1)
		And (Local1, 0x02, Local1)
		If (LEqual (Local1, 0x02))
		{
			Store (0x1F, Local0)
		}

		Store ("<----- BAT0: _STA", Debug)

		Return (Local0)
	}

	Method (_BIF, 0, Serialized)
	{
		Store ("-----> BAT0: _BIF", Debug)

		Store (B0FC, Index (PBIF, 0x02))
		Store (Divide (Multiply (B0FC, 6), 100), Index (PBIF, 0x05))
		Store (Divide (Multiply (B0FC, 3), 100), Index (PBIF, 0x06))

		Store ("<----- BAT0: _BIF", Debug)

		Return (PBIF)
	}

	Method (_BST, 0, Serialized)
	{
		Store ("-----> BAT0: _BST", Debug)

		Store (B0ST, Local0)
		And (Local0, 0x40, Local0)
		If (LEqual (Local0, 0x40))
		{
			If (LEqual (PWRS, 1))
			{
				Store (0x00, Index (PBST, 0x00))
			}
			Else
			{
				Store (0x01, Index (PBST, 0x00))
			}
		}
		Else
		{
			Store (0x02, Index (PBST, 0x00))
		}

		Store (B0AC, Local1)
		If (LGreaterEqual (Local1, 0x8000))
		{
			Subtract (0x00010000, Local1, Local1)
		}

		Store (Local1, Index (PBST, 0x01))
		Store (B0RC, Index (PBST, 0x02))
		Store (B0VT, Index (PBST, 0x03))

		Store ("<----- BAT0: _BST", Debug)

		Return (PBST)
	}
}
