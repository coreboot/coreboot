/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, EC_GPE_SWI)

	Name (ECOK, Zero)

	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
	}

	OperationRegion (ERAM, EmbeddedControl, Zero, 0xFF)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x7F),
		LSTE, 1,	/* lid state */
		, 7,
		ACEX, 1,	/* AC adapter present */
		BTEX, 1,	/* battery present */
		, 6,
		Offset (0x84),
		BTDC, 16,	/* battery design capacity - mAh */
		BTFV, 16,	/* battery last full voltage - mV */
		BTFC, 16,	/* battery last full capacity - mAh */
		Offset (0x8C),
		BTST, 3,	/* battery state */
		, 5,
		BTCR, 16,	/* battery present current - mA */
		BTRC, 16,	/* battery remaining capacity - mAh */
		BTVT, 16,	/* battery present voltage - mV */
		Offset (0xA3),
		DSPO, 8,	/* Display off - write 1 to power off display */
		BCST, 8,	/* battery charge start threshold - % */
		BCET, 8,	/* battery charge end threshold - % */
	}

	#include "button.asl"
	#include "ac.asl"
	#include "battery.asl"
	#include "vbtn.asl"

	Method (PTS, 1, Serialized) {
		Printf ("EC: PTS: %o", ToHexString(Arg0))
		If (ECOK) {
			// Power off display
			DSPO = One
		}
	}

	Method (WAK, 1, Serialized) {
		Printf ("EC: WAK: %o", ToHexString(Arg0))
		If (ECOK) {
			DSPO = Zero
			^AC.ACEX = ACEX
			Notify(BAT0, Zero)
			Notify(AC, Zero)
		}
	}

	Method (_Q54, 0, NotSerialized) // Power button press
	{
		Printf ("EC: _Q54: power button press")
		Notify (PWRB, 0x80)
	}

	Method (_Q0A, 0, NotSerialized) // Charger plugged or unplugged
	{
		Printf ("EC: _Q0A: charger state changed")
		If (ECOK) {
			^AC.ACEX = ACEX
		}
		Notify(BAT0, 0x81)	// Information change
		Notify(AC, 0x80)	// Status change
	}

	Method (_Q0B, 0, NotSerialized) // Battery status change
	{
		Printf ("EC: _Q0B: battery state changed")
		Notify(BAT0, 0x81)	// Information change
		Notify(BAT0, 0x80)	// Status change
	}

	/* There is a lid/cover sensor, but it is not reliable with a soft cover. */
	Method (_Q0C, 0, NotSerialized) // Cover closed
	{
		Printf ("EC: _Q0C: cover closed")
	}

	Method (_Q0D, 0, NotSerialized) // Cover opened
	{
		Printf ("EC: _Q0D: cover opened")
	}

	Method (_REG, 2, Serialized)  // _REG: Region Availability
	{
		Printf ("EC: _REG: %o, %o", Arg0, Arg1)
		If ((Arg0 == 0x03) && (Arg1 == One)) {
			// EC is now available
			ECOK = One

			// Set current AC and battery state
			^AC.ACEX = ACEX
			^BAT0.BTEX = BTEX

			// Notify of changes
			Notify(AC, Zero)
			Notify(BAT0, Zero)

			Printf ("EC is ready; BTEX=%o, ACEX=%o", BTEX, ACEX)
		}
	}
}
