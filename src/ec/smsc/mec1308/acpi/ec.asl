/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_GPE)	// GPE for Runtime SCI

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x80),
		BTEX, 1,		// Battery Exists
		, 1,
		ACEX, 1,		// AC Exists
		, 5,
		Offset (0x83),
		LIDS, 1,		// Lid Switch State
		, 7,
		BTST, 8,		// Battery State
		Offset (0xA2),
		BTRA, 16,		// Battery Remaining Capacity
		BTPR, 16,		// Battery Present Rate
		BTVO, 16,		// Battery Present Voltage
		Offset (0xB0),
		BTDA, 16,		// Battery Design Capacity
		BTDF, 16,		// Battery Last Full Charge Capacity
		BTDV, 16,		// Battery Design Voltage
		BTDL, 16,		// Battery Design Low
		Offset (0xC0),
		CPUT, 8,		// CPU Temperature
		Offset (0xCA),
		FSL0, 1,		// Fan Speed Level 0
		FSL1, 1,		// Fan Speed Level 1
		FSL2, 1,		// Fan Speed Level 2
		FSL3, 1,		// Fan Speed Level 3
		FSL4, 1,		// Fan Speed Level 4
		, 2,
		FCOS, 1,		// Fan Speed OS Control
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 0, 1)
			IO (Decode16, 0x66, 0x66, 0, 1)
		})
		Return (ECMD)
	}

	Method (_REG, 2, NotSerialized)
	{
		// Initialize AC power state
		\PWRS = ACEX

		// Initialize LID switch state
		\LIDS = LIDS

		// Enable OS control of fan speed
		FCOS = 1

		// Force a read of CPU temperature
		Local0 = CPUT
		/* So that we don't get a warning that Local0 is unused.  */
		Local0++
	}

	PowerResource (FNP0, 0, 0)
	{
		Method (_STA) {
			If (\FLVL <= 0) {
				Return (1)
			} Else {
				Return (Zero)
			}
		}
		Method (_ON)  {
			If (FCOS) {
				FSL0 = 1
				\FLVL = 0
				Notify (\_TZ.THRM, 0x81)
			}
		}
		Method (_OFF) {
			If (FCOS) {
				FSL0 = Zero
				\FLVL = 1
				Notify (\_TZ.THRM, 0x81)
			}
		}
	}

	PowerResource (FNP1, 0, 0)
	{
		Method (_STA) {
			If (\FLVL <= 1) {
				Return (1)
			} Else {
				Return (Zero)
			}
		}
		Method (_ON)  {
			If (FCOS) {
				FSL1 = 1
				\FLVL = 1
				Notify (\_TZ.THRM, 0x81)
			}
		}
		Method (_OFF) {
			If (FCOS) {
				FSL1 = Zero
				\FLVL = 2
				Notify (\_TZ.THRM, 0x81)
			}
		}
	}

	PowerResource (FNP2, 0, 0)
	{
		Method (_STA) {
			If (\FLVL <= 2) {
				Return (1)
			} Else {
				Return (Zero)
			}
		}
		Method (_ON)  {
			If (FCOS) {
				FSL2 = 1
				\FLVL = 2
				Notify (\_TZ.THRM, 0x81)
			}
		}
		Method (_OFF) {
			If (FCOS) {
				FSL2 = Zero
				\FLVL = 3
				Notify (\_TZ.THRM, 0x81)
			}
		}
	}

	PowerResource (FNP3, 0, 0)
	{
		Method (_STA) {
			If (\FLVL <= 3) {
				Return (1)
			} Else {
				Return (Zero)
			}
		}
		Method (_ON)  {
			If (FCOS) {
				FSL3 = 1
				\FLVL = 3
				Notify (\_TZ.THRM, 0x81)
			}
		}
		Method (_OFF) {
			If (FCOS) {
				FSL3 = Zero
				\FLVL = 4
				Notify (\_TZ.THRM, 0x81)
			}
		}
	}

	PowerResource (FNP4, 0, 0)
	{
		Method (_STA) {
			If (\FLVL <= 4) {
				Return (1)
			} Else {
				Return (Zero)
			}
		}
		Method (_ON)  {
			If (FCOS) {
				FSL4 = 1
				\FLVL = 4
				Notify (\_TZ.THRM, 0x81)
			}
		}
		Method (_OFF) {
			If (FCOS) {
				FSL4 = Zero
				\FLVL = 5
				Notify (\_TZ.THRM, 0x81)
			}
		}
	}

	Device (FAN0)
	{
		Name (_HID, EISAID ("PNP0C0B"))
		Name (_UID, 0)
		Name (_PR0, Package () { FNP0 })
	}

	Device (FAN1)
	{
		Name (_HID, EISAID ("PNP0C0B"))
		Name (_UID, 1)
		Name (_PR0, Package () { FNP1 })
	}

	Device (FAN2)
	{
		Name (_HID, EISAID ("PNP0C0B"))
		Name (_UID, 2)
		Name (_PR0, Package () { FNP2 })
	}

	Device (FAN3)
	{
		Name (_HID, EISAID ("PNP0C0B"))
		Name (_UID, 3)
		Name (_PR0, Package () { FNP3 })
	}

	Device (FAN4)
	{
		Name (_HID, EISAID ("PNP0C0B"))
		Name (_UID, 4)
		Name (_PR0, Package () { FNP4 })
	}

	// AC Power Connected
	Method (_Q51, 0, NotSerialized)
	{
		\PWRS = 1
		Notify (AC, 0x80)
		\PNOT ()
	}

	// AC Power Removed
	Method (_Q52, 0, NotSerialized)
	{
		\PWRS = Zero
		Notify (AC, 0x80)
		\PNOT ()
	}

	// Battery State Change
	Method (_Q53, 0, NotSerialized)
	{
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
	}

	// Battery State Change
	Method (_Q54, 0, NotSerialized)
	{
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
	}

	// Power State Change
	Method (_Q55, 0, NotSerialized)
	{
		\PNOT ()
	}

	// Lid Switch Event
	Method (_Q5E, 0, NotSerialized)
	{
		\LIDS = LIDS
		Notify (\_SB.LID0, 0x80)
	}

	// Lid Switch Event
	Method (_Q5F, 0, NotSerialized)
	{
		\LIDS = LIDS
		Notify (\_SB.LID0, 0x80)
	}

	#include "ac.asl"
	#include "battery.asl"
}
