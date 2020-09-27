/* SPDX-License-Identifier: GPL-2.0-only */

/* Arg0: Battery
 * Arg1: Battery Status Package
 * Arg2: charging
 * Arg3: discharging
 */
Method(BSTA, 4, NotSerialized)
{
	Acquire(ECLK, 0xffff)
	Store(0, Local0)

	Store(0, PAGE)

	Store(BAPR, Local2)

	if (Arg2) // charging
	{
		Or(2, Local0, Local0)

		If (LGreaterEqual (Local2, 0x8000)) {
			Store(0, Local2)
		}
	}

	if (Arg3) // discharging
	{
		Or(1, Local0, Local0)
		Subtract(0x10000, Local2, Local2)
	}

	Store(Local0, Index(Arg1, 0x00))

	Store(0, PAGE)
	Store(BARC, Index(Arg1, 2))
	Store(Local2, Index(Arg1, 1))

	Store(0, PAGE)
	Store(BAVO, Index(Arg1, 3))
	Release(ECLK)
	Return (Arg1)
}

Method(BINF, 2, Serialized)
{
	Acquire(ECLK, 0xffff)
	Store(0, PAGE)
	Store(BAFC, Local2)
	Store(1, PAGE)
	Store(BADC, Local1)

	Store(Local1, Index(Arg0, 1))	// Design Capacity
	Store(Local2, Index(Arg0, 2))	// Last full charge capacity
	Store(1, PAGE)
	Store(BADV, Index(Arg0, 4))	// Design Voltage
	Divide (Local2, 20, , Index(Arg0, 5)) // Warning capacity

	Store(1, PAGE)
	Store (BASN, Local0)
	Name (SERN, Buffer (0x06) { "     " })
	Store (4, Local1)
	While (Local0)
	{
		Divide (Local0, 0x0A, Local2, Local0)
		Add (Local2, 48, Index (SERN, Local1))
		Decrement (Local1)
	}
	Store (SERN, Index (Arg0, 10)) // Serial Number

	Name (TYPE, Buffer() { 0, 0, 0, 0, 0 })
	Store(4, PAGE)
	Store(BATY, TYPE)
	Store(TYPE, Index (Arg0, 11)) // Battery type
	Store(5, PAGE)
	Store(BAOE, Index (Arg0, 12)) // OEM information
	Store(2, PAGE)
	Store(BANA, Index (Arg0, 9))  // Model number
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
