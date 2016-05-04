/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

// Mainboard specific throttle handler
External (\_TZ.THRT, MethodObj)
External (\_SB.DPTF.TEVT, MethodObj)
#ifdef DPTF_ENABLE_CHARGER
External (\_SB.DPTF.TCHG, DeviceObj)
#endif

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI)
	Name (TOFS, EC_TEMP_SENSOR_OFFSET)
	Name (TNCA, EC_TEMP_SENSOR_NOT_CALIBRATED)
	Name (TNOP, EC_TEMP_SENSOR_NOT_POWERED)
	Name (TBAD, EC_TEMP_SENSOR_ERROR)
	Name (TNPR, EC_TEMP_SENSOR_NOT_PRESENT)
	Name (DWRN, 15)		// Battery capacity warning at 15%
	Name (DLOW, 10)		// Battery capacity low at 10%

	OperationRegion (ERAM, EmbeddedControl, 0x00, EC_ACPI_MEM_MAPPED_BEGIN)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x00),
		RAMV, 8,	// EC RAM Version
		TSTB, 8,	// Test Byte
		TSTC, 8,	// Complement of Test Byte
		KBLV, 8,	// Keyboard Backlight
		FAND, 8,	// Set Fan Duty Cycle
		PATI, 8,	// Programmable Auxiliary Trip Sensor ID
		PATT, 8,	// Programmable Auxiliary Trip Threshold
		PATC, 8,	// Programmable Auxiliary Trip Commit
		CHGL, 8,	// Charger Current Limit
	}

#if CONFIG_EC_GOOGLE_CHROMEEC_ACPI_MEMMAP
	OperationRegion (EMEM, EmbeddedControl,
			 EC_ACPI_MEM_MAPPED_BEGIN, EC_ACPI_MEM_MAPPED_SIZE)
	Field (EMEM, ByteAcc, Lock, Preserve)
#else
	OperationRegion (EMEM, SystemIO, EC_LPC_ADDR_MEMMAP, EC_MEMMAP_SIZE)
	Field (EMEM, ByteAcc, NoLock, Preserve)
#endif
	{
		#include "emem.asl"
	}

	Method (TINS, 1, Serialized)
	{
		Switch (ToInteger (Arg0))
		{
			Case (0) { Return (TIN0) }
			Case (1) { Return (TIN1) }
			Case (2) { Return (TIN2) }
			Case (3) { Return (TIN3) }
			Case (4) { Return (TIN4) }
			Case (5) { Return (TIN5) }
			Case (6) { Return (TIN6) }
			Case (7) { Return (TIN7) }
			Case (8) { Return (TIN8) }
			Case (9) { Return (TIN9) }
			Default  { Return (TIN0) }
		}
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16,
			    EC_LPC_ADDR_ACPI_DATA,
			    EC_LPC_ADDR_ACPI_DATA,
			    0, 1)
			IO (Decode16,
			    EC_LPC_ADDR_ACPI_CMD,
			    EC_LPC_ADDR_ACPI_CMD,
			    0, 1)
		})
		Return (ECMD)
	}

	Method (_REG, 2, NotSerialized)
	{
		// Initialize AC power state
		Store (ACEX, \PWRS)

		// Initialize LID switch state
		Store (LIDS, \LIDS)
	}

	/* Read requested temperature and check against EC error values */
	Method (TSRD, 1, Serialized)
	{
		Store (\_SB.PCI0.LPCB.EC0.TINS (Arg0), Local0)

		/* Check for sensor not calibrated */
		If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNCA)) {
			Return (Zero)
		}

		/* Check for sensor not present */
		If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNPR)) {
			Return (Zero)
		}

		/* Check for sensor not powered */
		If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNOP)) {
			Return (Zero)
		}

		/* Check for sensor bad reading */
		If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TBAD)) {
			Return (Zero)
		}

		/* Adjust by offset to get Kelvin */
		Add (\_SB.PCI0.LPCB.EC0.TOFS, Local0, Local0)

		/* Convert to 1/10 Kelvin */
		Multiply (Local0, 10, Local0)

		Return (Local0)
	}

	// Lid Closed Event
	Method (_Q01, 0, NotSerialized)
	{
		Store ("EC: LID CLOSE", Debug)
		Store (LIDS, \LIDS)
		Notify (\_SB.LID0, 0x80)
	}

	// Lid Open Event
	Method (_Q02, 0, NotSerialized)
	{
		Store ("EC: LID OPEN", Debug)
		Store (LIDS, \LIDS)
		Notify (\_SB.LID0, 0x80)
	}

	// Power Button
	Method (_Q03, 0, NotSerialized)
	{
		Store ("EC: POWER BUTTON", Debug)
		Notify (\_SB.PWRB, 0x80)
	}

	// AC Connected
	Method (_Q04, 0, NotSerialized)
	{
		Store ("EC: AC CONNECTED", Debug)
		Store (ACEX, \PWRS)
		Notify (AC, 0x80)
#ifdef DPTF_ENABLE_CHARGER
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
#endif
		\PNOT ()
	}

	// AC Disconnected
	Method (_Q05, 0, NotSerialized)
	{
		Store ("EC: AC DISCONNECTED", Debug)
		Store (ACEX, \PWRS)
		Notify (AC, 0x80)
#ifdef DPTF_ENABLE_CHARGER
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
#endif
		\PNOT ()
	}

	// Battery Low Event
	Method (_Q06, 0, NotSerialized)
	{
		Store ("EC: BATTERY LOW", Debug)
		Notify (BAT0, 0x80)
	}

	// Battery Critical Event
	Method (_Q07, 0, NotSerialized)
	{
		Store ("EC: BATTERY CRITICAL", Debug)
		Notify (BAT0, 0x80)
	}

	// Battery Info Event
	Method (_Q08, 0, NotSerialized)
	{
		Store ("EC: BATTERY INFO", Debug)
		Notify (BAT0, 0x81)
	}

	// Thermal Overload Event
	Method (_Q0A, 0, NotSerialized)
	{
		Store ("EC: THERMAL OVERLOAD", Debug)
		Notify (\_TZ, 0x80)
	}

	// Thermal Event
	Method (_Q0B, 0, NotSerialized)
	{
		Store ("EC: THERMAL", Debug)
		Notify (\_TZ, 0x80)
	}

	// USB Charger
	Method (_Q0C, 0, NotSerialized)
	{
		Store ("EC: USB CHARGER", Debug)
	}

	// Key Pressed
	Method (_Q0D, 0, NotSerialized)
	{
		Store ("EC: KEY PRESSED", Debug)
	}

	// Thermal Shutdown Imminent
	Method (_Q10, 0, NotSerialized)
	{
		Store ("EC: THERMAL SHUTDOWN", Debug)
		Notify (\_TZ, 0x80)
	}

	// Battery Shutdown Imminent
	Method (_Q11, 0, NotSerialized)
	{
		Store ("EC: BATTERY SHUTDOWN", Debug)
		Notify (BAT0, 0x80)
	}

	// Throttle Start
	Method (_Q12, 0, NotSerialized)
	{
		Store ("EC: THROTTLE START", Debug)
		If (CondRefOf (\_TZ.THRT)) {
			\_TZ.THRT (1)
		}
	}

	// Throttle Stop
	Method (_Q13, 0, NotSerialized)
	{
		Store ("EC: THROTTLE STOP", Debug)
		If (CondRefOf (\_TZ.THRT)) {
			\_TZ.THRT (0)
		}
	}

#ifdef EC_ENABLE_PD_MCU_DEVICE
	// PD event
	Method (_Q16, 0, NotSerialized)
	{
		Store ("EC: GOT PD EVENT", Debug)
		Notify (ECPD, 0x80)
	}
#endif

	// Battery Status
	Method (_Q17, 0, NotSerialized)
	{
		Store ("EC: BATTERY STATUS", Debug)
		Notify (BAT0, 0x80)
	}

	// MKBP interrupt.
	Method (_Q1A, 0, NotSerialized)
	{
		Store ("EC: MKBP", Debug)
		Notify (CREC, 0x80)
	}

	/*
	 * Dynamic Platform Thermal Framework support
	 */

	/* Mutex for EC PAT interface */
	Mutex (PATM, 1)

	/*
	 * Set Aux Trip Point 0
	 *   Arg0 = Temp Sensor ID
	 *   Arg1 = Value to set
	 */
	Method (PAT0, 2, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		/* Set sensor ID */
		Store (ToInteger (Arg0), ^PATI)

		/* Temperature is passed in 1/10 Kelvin */
		Divide (ToInteger (Arg1), 10, , Local1)

		/* Adjust by EC temperature offset */
		Subtract (Local1, ^TOFS, ^PATT)

		/* Set commit value with SELECT=0 and ENABLE=1 */
		Store (0x02, ^PATC)

		Release (^PATM)
		Return (1)
	}

	/*
	 * Set Aux Trip Point 1
	 *   Arg0 = Temp Sensor ID
	 *   Arg1 = Value to set
	 */
	Method (PAT1, 2, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		/* Set sensor ID */
		Store (ToInteger (Arg0), ^PATI)

		/* Temperature is passed in 1/10 Kelvin */
		Divide (ToInteger (Arg1), 10, , Local1)

		/* Adjust by EC temperature offset */
		Subtract (Local1, ^TOFS, ^PATT)

		/* Set commit value with SELECT=1 and ENABLE=1 */
		Store (0x03, ^PATC)

		Release (^PATM)
		Return (1)
	}

	/* Disable Aux Trip Points
	 *   Arg0 = Temp Sensor ID
	 */
	Method (PATD, 1, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		Store (ToInteger (Arg0), ^PATI)
		Store (0x00, ^PATT)

		/* Disable PAT0 */
		Store (0x00, ^PATC)

		/* Disable PAT1 */
		Store (0x01, ^PATC)

		Release (^PATM)
		Return (1)
	}

	/*
	 * Thermal Threshold Event
	 */
	Method (_Q09, 0, NotSerialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return ()
		}

		/* Read sensor ID for event */
		Store (^PATI, Local0)

		/* When sensor ID returns 0xFF then no more events */
		While (LNotEqual (Local0, EC_TEMP_SENSOR_NOT_PRESENT))
		{
			If (CondRefOf (\_SB.DPTF.TEVT)) {
				\_SB.DPTF.TEVT (Local0)
			}

			/* Keep reaading sensor ID for event */
			Store (^PATI, Local0)
		}

		Release (^PATM)
	}

	/*
	 * Set Charger Current Limit
	 *   Arg0 = Current Limit in 64mA steps
	 */
	Method (CHGS, 1, Serialized)
	{
		Store (ToInteger (Arg0), ^CHGL)
	}

	/*
	 * Disable Charger Current Limit
	 */
	Method (CHGD, 0, Serialized)
	{
		Store (0xFF, ^CHGL)
	}

	#include "ac.asl"
	#include "battery.asl"
	#include "cros_ec.asl"

#ifdef EC_ENABLE_ALS_DEVICE
	#include "als.asl"
#endif

#ifdef EC_ENABLE_KEYBOARD_BACKLIGHT
       #include "keyboard_backlight.asl"
#endif

#ifdef EC_ENABLE_PD_MCU_DEVICE
	#include "pd.asl"
#endif
}
