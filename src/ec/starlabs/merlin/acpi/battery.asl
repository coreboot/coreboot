/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(EC_STARLABS_MERLIN)
Method (_Q0B, 0, NotSerialized)			// Event: Battery Information Update
{
	Notify (BAT0, 0x80)
}
#endif

Device (BAT0)
{
	Name (_HID, EisaId("PNP0C0A"))
	Name (_UID, 0)
	Method (_STA, 0, NotSerialized)
	{
		// Battery Status
		// 0x80 BIT1 0x01 = Present
		// 0x80 BIT1 0x00 = Not Present
		If (ECRD (RefOf(ECPS)) & 0x02)
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

	Method (BFCX, 0, NotSerialized)
	{
		Local0 = ECRD(RefOf(B1FC))
		If (Local0) {
			If (Local0 != 0xffff) {
				Return (Local0)
			}
		}
		Return (ECRD(RefOf(B1DC)))
	}

	Method (_BIF, 0, NotSerialized)
	{
		Local0 = ECRD(RefOf(B1DC))
		If (Local0) {
			Local1 = BFCX()
			SBIF  [1] = Local0
			SBIF  [2] = Local1
			SBIF  [4] = ECRD(RefOf(B1DV))
			SBIF  [5] = (Local1 + 2) / 5	// 20%
			SBIF  [6] = (Local1 + 5) / 10	// 10%
			SBIF  [7] = Local1 / 500	// 0.2%
			SBIF  [8] = Local1 / 500	// 0.2%
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
		Local0 = ECRD(RefOf(B1DC))
		If (Local0) {
			Local1 = BFCX()
			XBIF  [2] = Local0
			XBIF  [3] = Local1
			XBIF  [5] = ECRD(RefOf(B1DV))
			XBIF  [6] = (Local1 + 2) / 5	// 20%
			XBIF  [7] = (Local1 + 5) / 10	// 10%
			If (B1CC != 0xffff) {
				XBIF  [8] = ECRD(RefOf(B1CC))
			}
			XBIF [14] = Local1 / 500	// 0.2%
			XBIF [15] = Local1 / 500	// 0.2%
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
		PKG1[0] = (ECRD(RefOf(B1ST)) & 0x0f)
		PKG1[1] = ECRD(RefOf(B1PR))

		PKG1[2] = 0xffffffff
		Local2 = 0

		Local0 = ECRD(RefOf(B1RP))
		If (Local0 <= 100) {
			Local1 = BFCX()
			If (Local1) {
				If (Local1 != 0xffff) {
					PKG1[2] = ((Local0 * Local1) + 50) / 100
					Local2 = 1
				}
			}
		}
		If (Local2 == 0) {
			Local0 = ECRD(RefOf(B1RC))
			If (Local0 != 0xffff) {
				Local1 = BFCX()
				If (Local1) {
					If (Local1 != 0xffff) {
						If (Local0 > Local1) {
							Local0 = Local1
						}
					}
				}
				PKG1[2] = Local0
			}
		}
		PKG1[3] = ECRD(RefOf(B1PV))
		Return (PKG1)
	}
	Method (_PCL, 0, NotSerialized)
	{
		Return (
			Package() { _SB }
		)
	}
}
