/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT0)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	// Battery Slot Status
	Method (_STA, 0, Serialized)
	{
		If (ECWR & 0x02)
		{
			Return (0x1F)
		}
		Return (0x0F)
	}

	// Default Static Battery Information
	Name (BPKG, Package (13)
	{
		1,		//  0: Power Unit
		0xFFFFFFFF,	//  1: Design Capacity
		0xFFFFFFFF,	//  2: Last Full Charge Capacity
		1,		//  3: Battery Technology(Rechargeable)
		0xFFFFFFFF,	//  4: Design Voltage 10.8V
		0,		//  5: Design capacity of warning
		0,		//  6: Design capacity of low
		0x64,		//  7: Battery capacity granularity 1
		0,		//  8: Battery capacity granularity 2
		"CN6613-2S3P",	//  9: Model Number
		"6UA3",		// 10: Serial Number
		"Real",		// 11: Battery Type
		"GDPT"		// 12: OEM Information
	})

	Method (_BIF, 0, Serialized)
	{
		BPKG[1] = B1DC
		BPKG[2] = B1FC
		BPKG[4] = B1FV
		If (B1FC)
		{
			BPKG[5] = B1FC / 10
			BPKG[6] = B1FC / 25
			BPKG[7] = B1DC / 100
		}

		Return (BPKG)
	}

	Name (PKG1, Package (4)
	{
		0xFFFFFFFF,	// Battery State
		0xFFFFFFFF,	// Battery Present Rate
		0xFFFFFFFF,	// Battery Remaining Capacity
		0xFFFFFFFF,	// Battery Present Voltage
	})

	Method (_BST, 0, Serialized)
	{


		PKG1[0] = B1ST & 0x07
		If (B1ST & 0x01)
		{
			PKG1[1] = B1CR
		}
		Else
		{
			PKG1[1] = B1CR
		}
		PKG1[2] = B1RC
		PKG1[3] = B1VT
		Return (PKG1)
	}
}
