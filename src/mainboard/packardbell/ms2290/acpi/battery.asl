/* SPDX-License-Identifier: GPL-2.0-only */

/* Arg0: Battery
 * Arg1: Battery Status Package
 * Arg2: charging
 * Arg3: discharging
 */
Method(BSTA, 4, NotSerialized)
{
	Acquire(ECLK, 0xffff)
	Local0 = 0

	PAGE = 0

	Local2 = BAPR

	if (Arg2) // charging
	{
		Local0 |= 2

		If (Local2 == 0x8000) {
			Local2 = 0
		}
	}

	if (Arg3) // discharging
	{
		Local0 |= 1
		Local2 = 0x10000 - Local2
	}

	Arg1[0] = Local0

	PAGE = 0
	Arg1[2] = BARC
	Arg1[1] = Local2

	PAGE = 0
	Arg1[3] = BAVO
	Release(ECLK)
	Return (Arg1)
}

Method(BINF, 2, Serialized)
{
	Acquire(ECLK, 0xffff)
	PAGE = 0
	Local2 = BAFC
	PAGE = 1
	Local1 = BADC

	Arg0[1] = Local1	// Design Capacity
	Arg0[2] = Local2	// Last full charge capacity
	PAGE = 1
	Arg0[4] = BADV		// Design Voltage
	Arg0[5] = Local2 / 20	// Warning capacity

	PAGE = 1
	Local0 = BASN
	Name (SERN, Buffer (0x06) { "     " })
	Local1 = 4
	While (Local0)
	{
		Local2 = Local0
		Local0 /= 0x0A
		Local2 -= (Local0 * 0x0A)
		SERN[Local1] = Local2 + 48
		Local1--
	}
	Arg0[10] = SERN // Serial Number

	Name (TYPE, Buffer() { 0, 0, 0, 0, 0 })
	PAGE = 4
	TYPE = BATY
	Arg0[11] = TYPE // Battery type
	PAGE = 5
	Arg0[12] = BAOE // OEM information
	PAGE = 2
	Arg0[9] = BANA  // Model number
	Release(ECLK)
	Return (Arg0)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x00)
	Name (_PCL, Package () { \_SB })

	Name (BATS, Package ()
	{
		0x00,			// 0: PowerUnit: Report in mWh
		0xFFFFFFFF,		// 1: Design cap
		0xFFFFFFFF,		// 2: Last full charge cap
		0x01,			// 3: Battery Technology
		10800,			// 4: Design Voltage (mV)
		0x00,			// 5: Warning design capacity
		200,			// 6: Low design capacity
		1,			// 7: granularity1
		1,			// 8: granularity2
		"",			// 9: Model number
		"",			// A: Serial number
		"",			// B: Battery Type
		""			// C: OEM information
	})

	Method (_BIF, 0, NotSerialized)
	{
		Return (BINF(BATS, 0))
	}

	Name (BATI, Package ()
	{
		0,			// Battery State
					// Bit 0 - discharge
					// Bit 1 - charge
					// Bit 2 - critical state
		0,			// Battery present Rate
		0,			// Battery remaining capacity
		0			// Battery present voltage
	})

	Method (_BST, 0, NotSerialized)
	{
		if (B0PR) {
			Return (BSTA(0, BATI, B0CH, B0DI))
		} else {
			Return (BATS)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		if (B0PR) {
			Return (0x1f)
		} else {
			Return (0x0f)
		}
	}
}

/* Battery attach/detach */
Method(_Q40, 0, NotSerialized)
{
	Notify(BAT0, 0x81)
}
Method(_Q41, 0, NotSerialized)
{
	Notify(BAT0, 0x81)
}

Method(_Q48, 0, NotSerialized)
{
	Notify(BAT0, 0x80)
}
Method(_Q4C, 0, NotSerialized)
{
	Notify(BAT0, 0x80)
}
