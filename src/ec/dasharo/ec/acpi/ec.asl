/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB) {
	#include "ac.asl"
	#include "battery.asl"
	#include "buttons.asl"
	#include "dasharo.asl"
	#include "dtt.asl"
	#include "hid.asl"
	#include "lid.asl"
}

Device (\_SB.PCI0.LPCB.EC0)
{
	Name (_HID, EisaId ("PNP0C09") /* Embedded Controller Device */)
	Name (_GPE, EC_GPE_SCI)  // _GPE: General Purpose Events
	Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
	{
		IO (Decode16,
			0x0062,             // Range Minimum
			0x0062,             // Range Maximum
			0x00,               // Alignment
			0x01,               // Length
			)
		IO (Decode16,
			0x0066,             // Range Minimum
			0x0066,             // Range Maximum
			0x00,               // Alignment
			0x01,               // Length
			)
	})

	#include "ec_ram.asl"

	Name (ECOK, 0)
	Method (_REG, 2, Serialized)  // _REG: Region Availability
	{
		Printf ("EC: _REG %o %o", ToHexString(Arg0), ToHexString(Arg1))
		If ((Arg0 == 0x03) && (Arg1 == 1)) {
			// Enable hardware touchpad lock and airplane mode keys
			ECOS = 1

			// Enable software display brightness keys
			WINF = 1

			// Set current AC state
			^^^^AC.ACFG = ADP
			// Update battery information and status
			^^^^BAT0.UPBI()
			^^^^BAT0.UPBS()

			// Notify of changes
			Notify(^^^^AC, 0)
			Notify(^^^^BAT0, 0)

			PNOT ()

			// EC is now available
			ECOK = Arg1
		}
	}

	Name (S3OS, 0)
	Method (PTS, 1, Serialized) {
		Printf ("EC: PTS: %o", ToHexString(Arg0))
		If (ECOK) {
			// Save ECOS during sleep
			S3OS = ECOS

			// Clear wake cause
			WFNO = 0
		}
	}

	Method (WAK, 1, Serialized) {
		Printf ("EC: WAK: %o", ToHexString(Arg0))
		If (ECOK) {
			// Restore ECOS after sleep
			ECOS = S3OS

			// Set current AC state
			^^^^AC.ACFG = ADP

			// Update battery information and status
			^^^^BAT0.UPBI()
			^^^^BAT0.UPBS()

			// Notify of changes
			Notify(^^^^AC, 0)
			Notify(^^^^BAT0, 0)
		}
	}

	Method (S0IX, 1, Serialized) {
		Printf ("EC: S0ix hook")
		If (ECOK) {
			S0XH = Arg0
		}
	}

	Method (EDSX, 1, Serialized) {
		Printf ("EC: Display hook")
		If (ECOK) {
			DSPH = Arg0
		}
	}

	Method (_Q0A, 0, NotSerialized) // Touchpad Toggle
	{
		Printf ("EC: Touchpad Toggle")
	}

	Method (_Q0B, 0, NotSerialized) // Screen Toggle
	{
		Printf ("EC: Screen Toggle")
	}

	Method (_Q0C, 0, NotSerialized)  // Mute
	{
		Printf ("EC: Mute")
	}

	Method (_Q0E, 0, NotSerialized) // Volume Down
	{
		Printf ("EC: Volume Down")
	}

	Method (_Q0F, 0, NotSerialized) // Volume Up
	{
		Printf ("EC: Volume Up")
	}

	Method (_Q10, 0, NotSerialized) // Switch Video Mode
	{
		Printf ("EC: Switch Video Mode")
	}

	Method (_Q11, 0, NotSerialized) // Brightness Down
	{
		Printf ("EC: Brightness Down")
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (20)
		}
	}

	Method (_Q12, 0, NotSerialized) // Brightness Up
	{
		Printf ("EC: Brightness Up")
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (19)
		}
	}

	Method (_Q13, 0, NotSerialized) // Camera Toggle
	{
		Printf ("EC: Camera Toggle")
	}

	Method (_Q14, 0, NotSerialized) // Airplane Mode
	{
		Printf ("EC: Airplane Mode")
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (8)
		}
		// TODO: hardware airplane mode
	}

	Method (_Q15, 0, NotSerialized) // Suspend Button
	{
		Printf ("EC: Suspend Button")
		Notify (SLPB, 0x80)
	}

	Method (_Q16, 0, NotSerialized) // AC Detect
	{
		Printf ("EC: AC Detect")
		^^^^AC.ACFG = ADP
		Notify (AC, 0x80) // Status Change
		If (BAT0)
		{
			Notify (^^^^BAT0, 0x81) // Information Change
			Notify (^^^^BAT0, 0x80) // Status Change
		}
	}

	Method (_Q17, 0, NotSerialized)  // BAT0 Update
	{
		Printf ("EC: BAT0 Update (17)")
		Notify (^^^^BAT0, 0x81) // Information Change
	}

	Method (_Q19, 0, NotSerialized)  // BAT0 Update
	{
		Printf ("EC: BAT0 Update (19)")
		Notify (^^^^BAT0, 0x81) // Information Change
	}

	Method (_Q1B, 0, NotSerialized) // Lid Close
	{
		Printf ("EC: Lid Close")
		Notify (LID0, 0x80)
	}

	Method (_Q1C, 0, NotSerialized) // Thermal Trip
	{
		Printf ("EC: Thermal Trip")
		/* TODO
		Notify (\_TZ.TZ0, 0x81) // Thermal Trip Point Change
		Notify (\_TZ.TZ0, 0x80) // Thermal Status Change
		*/
	}

	Method (_Q1D, 0, NotSerialized) // Power Button
	{
		Printf ("EC: Power Button")
	}

	Method (_Q50, 0, NotSerialized) // Other Events
	{
		Local0 = OEM4
		Printf ("EC: Other: %o", ToHexString(Local0))
	}

	#if CONFIG(EC_DASHARO_EC_BAT_THRESHOLDS)
	#include "battery_thresholds.asl"
	#endif
}
