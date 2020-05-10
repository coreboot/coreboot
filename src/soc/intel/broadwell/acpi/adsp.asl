/* SPDX-License-Identifier: GPL-2.0-only */

Device (ADSP)
{
	Method (_HID, 0, Serialized)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3438")
		}

		// LynxPoint-LP
		Return ("INT33C8")
	}
	Name (_UID, 1)
	Name (_DDN, "Intel Smart Sound Technology")

	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00100000, BAR0)
		Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR1)
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {3}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR address and length if set in NVS
		If (LNotEqual (\S8B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B8A0)
			CreateDwordField (^RBUF, ^BAR1._BAS, B8A1)
			Store (\S8B0, B8A0)
			Store (\S8B1, B8A1)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S8EN, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	Device (I2S0)
	{
		Name (_ADR, 0)
	}

	Device (I2S1)
	{
		Name (_ADR, 1)
	}
}
