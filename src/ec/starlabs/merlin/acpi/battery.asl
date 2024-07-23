/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT0)
{
	Name (_HID, EisaId("PNP0C0A"))
	Name (_UID, 0)
	Method (_STA, 0, NotSerialized)
	{
		// Battery Status
		// 0x80 BIT1 0x01 = Present
		// 0x80 BIT1 0x00 = Not Present
		If (ECPS & 0x02)
		{
			Return (0x1F)
		}
		Return (0x0F)
	}

	Name (SBIF, Package (13)
	{
		1,					//  0: Power Unit			0:	mWh
							//					1:	mA
		0xffffffff,				//  1: Design Capacity
		0xffffffff,				//  2: Last Full Charge Capacity
		1,					//  3: Battery Technology		0:	Primary (non-rechargeable)
							//					1:	Secondary (rechargeable)
		0xffffffff,				//  4: Design Voltage
		0,					//  5: Design capacity of Warning
		0,					//  6: Design capacity of Low
		0xffffffff,				//  7: Battery capacity Decrement Granularity
		0xffffffff,				//  8: Battery capacity Increment Granularity
		CONFIG_EC_STARLABS_BATTERY_MODEL,	//  9: Model Number
		"Unknown",				// 10: Serial Number
		CONFIG_EC_STARLABS_BATTERY_TYPE,	// 11: Battery Type
		CONFIG_EC_STARLABS_BATTERY_OEM		// 12: OEM Information
	})
	Method (_BIF, 0, NotSerialized)
	{
		If (B1DC) {
			SBIF  [1] = B1DC
			If (B1FC != 0xffff) {
				SBIF  [2] = B1FC
			} Else {
				SBIF  [2] = B1DC
			}
			SBIF  [4] = B1DV
			SBIF  [5] = B1DC / 5	// 20%
			SBIF  [6] = B1DC / 20	// 5%
			SBIF  [7] = B1DC / 500	// 0.2%
			SBIF  [8] = B1DC / 500	// 0.2%
			If (B1SN != 0xffff) {
				SBIF [10] = B1SN
			}
		}
		Return (SBIF)
	}

	Name (XBIF, Package (21)
	{
		1,					//  0: Revision				0:	3.0
							//					1:	4.0
		1,					//  1: Power Unit			0:	mWh
							//					1:	mA
		0xffffffff,				//  2: Design Capacity
		0xffffffff,				//  3: Last Full Charge Capacity
		1,					//  4: Battery Technology		0:	Primary (non-rechargeable)
							//					1:	Secondary (rechargeable)
		0xffffffff,				//  5: Design Voltage
		0xffffffff,				//  6: Design Capacity of Warning
		0xffffffff,				//  7: Design Capacity of Low
		0xffffffff,				//  8: Cycle Count
		2,					//  9: Measurement Accuracy
		5000,					// 10: Max Sampling Time (ms)
		1000,					// 11: Min Sampling Time (ms)
		5000,					// 12: Max Averaging Interval
		1000,					// 13: Min Averaging Interval
		0xffffffff,				// 14: Battery Capacity Decrement Granularity
		0xffffffff,				// 15: Battery Capacity Increment Granularity
		CONFIG_EC_STARLABS_BATTERY_MODEL,	// 16: Model Number
		"Unknown",				// 17: Serial Number
		CONFIG_EC_STARLABS_BATTERY_TYPE,	// 18: Battery Type
		CONFIG_EC_STARLABS_BATTERY_OEM,		// 19: OEM Information
		1,					// 20: Swapping Capability		0:	Non swappable
							//					1:	Cold swappable
							//					16:	Hot swappable
	})
	Method (_BIX, 0, NotSerialized)
	{
		If (B1DC) {
			XBIF  [2] = B1DC
			If (B1FC != 0xffff) {
				XBIF  [3] = B1FC
			} Else {
				XBIF  [3] = B1DC
			}
			XBIF  [5] = B1DV
			XBIF  [6] = B1DC / 5	// 20%
			XBIF  [7] = B1DC / 20	// 5%
			If (B1CC != 0xffff) {
				XBIF  [8] = B1CC
			}
			XBIF [14] = B1DC / 500	// 0.2%
			XBIF [15] = B1DC / 500	// 0.2%
			If (B1SN != 0xffff) {
				XBIF [17] = B1SN
			}
		}
		Return (XBIF)
	}

	Name (PKG1, Package (4)
	{
		0xffffffff,	//  0: Battery State
		0xffffffff,	//  1: Battery Present Rate
		0xffffffff,	//  2: Battery Remaining Capacity
		0xffffffff,	//  3: Battery Present Voltage
	})
	Method (_BST, 0, NotSerialized)
	{
		PKG1[0] = (B1ST & 0x07)
		PKG1[1] = B1PR
		If (B1RC != 0xffff) {
			PKG1[2] = B1RC
		} Else {
			PKG1[2] = (B1RP * B1DC) / 100
		}
		PKG1[3] = B1PV
		Return (PKG1)
	}
	Method (_PCL, 0, NotSerialized)
	{
		Return (
			Package() { _SB }
		)
	}
}
