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
		0xffffffff,	// Battery Present Rate
		0xffffffff,	// Battery Remaining Capacity
		0xffffffff,	// Battery Present Voltage
	})


	// Battery Slot Status
	Method (_STA, 0, Serialized)
	{
		Printf ("-----> BAT0: _STA")

		Local0 = 0x0f

		Local1 = ECPS & 0x02
		If (Local1 == 0x02)
		{
			Local0 = 0x1f
		}

		Printf ("<----- BAT0: _STA")

		Return (Local0)
	}

	Method (_BIF, 0, Serialized)
	{
		Printf ("-----> BAT0: _BIF")

		PBIF[2] = B0FC
		PBIF[5] = (B0FC * 6) / 100
		PBIF[6] = (B0FC * 3) / 100

		Printf ("<----- BAT0: _BIF")

		Return (PBIF)
	}

	Method (_BST, 0, Serialized)
	{
		Printf ("-----> BAT0: _BST")

		Local0 = B0ST & 0x40
		If (Local0 == 0x40)
		{
			If (PWRS == 1)
			{
				PBST[0] = 0x00
			}
			Else
			{
				PBST[0] = 0x01
			}
		}
		Else
		{
			PBST[0] = 0x02
		}

		Local1 = B0AC
		If (Local1 >= 0x8000)
		{
			Local1 = 0x00010000 - Local1
		}

		PBST[1] = Local1
		PBST[2] = B0RC
		PBST[3] = B0VT

		Printf ("<----- BAT0: _BST")

		Return (PBST)
	}
}
