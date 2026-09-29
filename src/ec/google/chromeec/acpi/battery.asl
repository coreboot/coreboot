/* SPDX-License-Identifier: GPL-2.0-only */

// Scope (EC0)

/* Mutex for EC battery index interface */
Mutex (BATM, 0)

// Wait for desired battery index to be presented in shared memory
//   Arg0 = battery index
//   Returns 0 on success, 1 on error.
Method (BTSW, 1)
{
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
	If (BTIX == Arg0) {
		Return (0)
	}
	If (Arg0 >= BTCN) {
		Return (1)
	}
	^BTID = Arg0
	Local0 = 5      // Timeout 5 msec
	While (BTIX != Arg0)
	{
		Sleep (1)
		Local0--
		If (Local0 == 0)
		{
			Return (1)
		}
	}
#else
	If (Arg0 != 0) {
		Return (1)
	}
#endif
	Return (0)
}

// _STA implementation.
//   Arg0 = battery index
Method (BSTA, 1, Serialized)
{
#if CONFIG(SYSTEM_TYPE_MINIPC) || CONFIG(SYSTEM_TYPE_ALL_IN_ONE)
	Return (0)
#else
	If (Acquire (^BATM, 1000)) {
		Return (0)
	}

	If (!BTSW (Arg0) && BTEX) {
		Local0 = 0x1F
	} Else {
		Local0 = 0x0F
	}

	Release (^BATM)
	Return (Local0)
#endif
}

#if CONFIG(EC_GOOGLE_CHROMEEC_READ_BATTERY_LONG_STRING)
// Cached flag for BSRF FIFO readout support from EC.
Name(BRSS, 0xff)
#else
Name(BRSS, 0x0)
#endif
// Cached battery string response indicator
Name(BRI1, 0)
Name(BRI2, 0)
Name(BRI3, 0)
// Cached battery string response data to save suspend-resume time
Name(BRS1, Buffer(32) {0})
Name(BRS2, Buffer(32) {0})
Name(BRS3, Buffer(32) {0})
// Read extended battery strings from the selected battery.
//   Arg0 = string index
//
// If supported by the EC, strings of arbitrary length are read using the
// FIFO readout method. Otherwise short (no more than 8 bytes) strings are
// read from the EC shared memory map. The desired battery index should be
// selected with BTSW before calling this method.
//
// Currently supported string indexes:
//  * 1 = EC_ACPI_MEM_STRINGS_FIFO_ID_BATTERY_MODEL: battery model name,
//        equivalent to BMOD.
//  * 2 = EC_ACPI_MEM_STRINGS_FIFO_ID_BATTERY_SERIAL: battery serial number,
//        equivalent to BSER.
//  * 3 = EC_ACPI_MEM_STRINGS_FIFO_ID_BATTERY_MANUFACTURER: battery
//        manufacturer's name, equivalent to BMFG.
//
// These are assumed to be supported if the EC reports at least version 1 of
// string readout (it returns an integer greater than 0 and less than 255 when
// reading FIFO index 0). Future strings may require higher FIFO versions.
Method(BRSX, 1, Serialized)
{
	// It doesn't make sense to read the FIFO support indicator.
	if (Arg0 == 0 || Arg0 > 3)
	{
		Return ("")
	}

	// Check if response is already cached
	If (Arg0 == 1 && BRI1 == 1)
	{
		Return (BRS1) /* battery model name */
	}

	If (Arg0 == 2 && BRI2 == 1)
	{
		Return (BRS2) /* battery serial number */
	}

	If (Arg0 == 3 && BRI3 == 1)
	{
		Return (BRS3) /* battery manufacturer's name */
	}

	If (BRSS == 0xff)
	{
		// Write 0 to BSRF to read back a support indicator; nonzero and
		// non-0xFF if FIFO readout is supported, assuming minimum v1 support
		// for strings 1 through 3.
		BSRF = 0
		BRSS = BSRF

		// 0xff readback also means no support for FIFO readout, when the EC
		// doesn't even know what this command is.
		if (BRSS == 0xff)
		{
			BRSS = 0
		}
	}

	// If FIFO readout through BSRF is not supported, fall back to reading
	// the short strings in EMEM.
	If (BRSS == 0)
	{
		If (Arg0 == 1)
		{
			Local0 = ToString (Concatenate (BMOD, 0))
		}
		ElseIf (Arg0 == 2)
		{
			Local0 = ToString (Concatenate (BSER, 0))
		}
		ElseIf (Arg0 == 3)
		{
			Local0 = ToString (Concatenate (BMFG, 0))
		}
	}
	Else
	{
		// Select requested parameter to read
		BSRF = Arg0

		// Read to end of string, or up to a reasonable maximum length. Reads of
		// BSRF consume bytes from the FIFO, so take care to read it only once
		// per byte of data.
		Local0 = Buffer (33){}
		For (Local2 = 0, Local2 < SizeOf(Local0) - 1, Local2++)
		{
			Local1 = BSRF
			If (Local1 == 0)
			{
				Break
			}
			Local0[Local2] = Local1
		}

		Local0[Local2] = 0
		Local0 = ToString(Local0)
	}

	// Store the result in the cache
	If (Arg0 == 1)
	{
		BRS1 = Local0
		BRI1 = 1
	}

	If (Arg0 == 2)
	{
		BRS2 = Local0
		BRI2 = 1
	}

	If (Arg0 == 3)
	{
		BRS3 = Local0
		BRI3 = 1
	}

	Return (Local0)
}

// _BIX implementation.
//   Arg0 = battery index
//   Arg1 = PBIX
Method (BBIX, 2, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Arg1)
	}

	If (BTSW (Arg0)) {
		Release (^BATM)
		Return (Arg1)
	}
	// Last Full Charge Capacity
	Arg1[3] = BTDF

	// Design Voltage
	Arg1[5] = BTDV

	// Design Capacity
	Local0 = BTDA
	Arg1[2] = Local0

	// Design Capacity of Warning
	Local2 = Local0 * DWRN / 100
	Arg1[6] = Local2

	// Design Capacity of Low
	Local2 = Local0 * DLOW / 100
	Arg1[7] = Local2

	// Cycle Count
	Arg1[8] = BTCC

	// Get battery info from mainboard
	Arg1[16] = BRSX (1)
	Arg1[17] = BRSX (2)
	Arg1[19] = BRSX (3)

	Release (^BATM)
	Return (Arg1)
}

// _BIF implementation, derived from the _BIX data.
//   Arg0 = battery index
//   Arg1 = PBIF
//   Arg2 = PBIX
Method (BBIF, 3, Serialized)
{
	Local0 = BBIX (Arg0, Arg2)

	Arg1[1] = DeRefOf (Local0[2])	// Design Capacity
	Arg1[2] = DeRefOf (Local0[3])	// Last Full Charge Capacity
	Arg1[4] = DeRefOf (Local0[5])	// Design Voltage
	Arg1[5] = DeRefOf (Local0[6])	// Design Capacity of Warning
	Arg1[6] = DeRefOf (Local0[7])	// Design Capacity of Low
	Arg1[9] = DeRefOf (Local0[16])	// Model Number
	Arg1[10] = DeRefOf (Local0[17])	// Serial Number
	Arg1[12] = DeRefOf (Local0[19])	// OEM Information

	Return (Arg1)
}

// _BST implementation.
//   Arg0 = battery index
//   Arg1 = PBST
//   Arg2 = BSTP
Method (BBST, 3, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Arg1)
	}

	If (BTSW (Arg0)) {
		Release (^BATM)
		Return (Arg1)
	}
	//
	// 0: BATTERY STATE
	//
	// bit 0 = discharging
	// bit 1 = charging
	// bit 2 = critical level
	//
	Local1 = 0

	// Check if AC is present
	If (ACEX) {
		If (BFCG) {
			Local1 = 0x02
		} ElseIf (BFDC) {
			Local1 = 0x01
		}
	} Else {
		// Always discharging when on battery power
		Local1 = 0x01
	}

	// Check for critical battery level
	If (BFCR) {
		Local1 |= 4
	}
	Arg1[0] = Local1

	// Notify if battery state has changed since last time
	If (Local1 != DeRefOf (Arg2)) {
		Arg2 = Local1
		If (Arg0 == 0) {
			Notify (BAT0, 0x80)
		}
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
		Else {
			Notify (BAT1, 0x80)
		}
#endif
	}

	//
	// 1: BATTERY PRESENT RATE
	//
	Arg1[1] = BTPR

	//
	// 2: BATTERY REMAINING CAPACITY
	//
	Local1 = BTRA
#if CONFIG(EC_GOOGLE_CHROMEEC_NEEDS_BATTERY_WORKAROUND)
	// Workaround for full battery status on older boards
	// lacking EC compensation.
	If (ACEX && !(BFDC && BFCG)) {
		// On AC power and battery is neither charging
		// nor discharging.  Linux expects a full battery
		// to report same capacity as last full charge.
		// https://bugzilla.kernel.org/show_bug.cgi?id=12632
		Local2 = BTDF

		// See if within ~6% of full
		Local3 = Local2 >> 4
		If (Local1 > Local2 - Local3 && Local1 < Local2 + Local3)
		{
			Local1 = Local2
		}
	}
#endif
	Arg1[2] = Local1

	//
	// 3: BATTERY PRESENT VOLTAGE
	//
	Arg1[3] = BTVO

	Release (^BATM)
	Return (Arg1)
}

#define BATTERY_NAME	BAT0
#define BATTERY_INDEX	0
#define BATTERY_UID	1
#include "battery_device.asl"

#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
#define BATTERY_NAME	BAT1
#define BATTERY_INDEX	1
#define BATTERY_UID	2
#include "battery_device.asl"
#endif
