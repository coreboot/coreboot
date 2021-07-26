/* SPDX-License-Identifier: GPL-2.0-only */

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
		Printf ("-----> BAT0: _STA")

		Store (0x0F, Local0)

		Store (ECPS, Local1)
		And (Local1, 0x02, Local1)
		If (LEqual (Local1, 0x02))
		{
			Store (0x1F, Local0)
		}

		Printf ("<----- BAT0: _STA")

		Return (Local0)
	}

	Method (_BIF, 0, Serialized)
	{
		Printf ("-----> BAT0: _BIF")

		Store (B0FC, PBIF[2])
		Store (Divide (Multiply (B0FC, 6), 100), PBIF[5])
		Store (Divide (Multiply (B0FC, 3), 100), PBIF[6])

		Printf ("<----- BAT0: _BIF")

		Return (PBIF)
	}

	Method (_BST, 0, Serialized)
	{
		Printf ("-----> BAT0: _BST")

		Store (B0ST, Local0)
		And (Local0, 0x40, Local0)
		If (LEqual (Local0, 0x40))
		{
			If (LEqual (PWRS, 1))
			{
				Store (0x00, PBST[0])
			}
			Else
			{
				Store (0x01, PBST[0])
			}
		}
		Else
		{
			Store (0x02, PBST[0])
		}

		Store (B0AC, Local1)
		If (LGreaterEqual (Local1, 0x8000))
		{
			Subtract (0x00010000, Local1, Local1)
		}

		Store (Local1, PBST[1])
		Store (B0RC, PBST[2])
		Store (B0VT, PBST[3])

		Printf ("<----- BAT0: _BST")

		Return (PBST)
	}
}
