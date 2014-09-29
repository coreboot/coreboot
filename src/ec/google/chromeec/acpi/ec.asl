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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

// Mainboard specific throttle handler
External (\_TZ.THRT, MethodObj)
External (\_SB.DPTF.TEVT, MethodObj)
External (\_SB.DPTF.TCHG, DeviceObj)

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

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
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

	OperationRegion (EMEM, SystemIO, EC_LPC_ADDR_MEMMAP, EC_MEMMAP_SIZE)
	Field (EMEM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x00),
		TIN0, 8,	// Temperature 0
		TIN1, 8,	// Temperature 1
		TIN2, 8,	// Temperature 2
		TIN3, 8,	// Temperature 3
		TIN4, 8,	// Temperature 4
		TIN5, 8,	// Temperature 5
		TIN6, 8,	// Temperature 6
		TIN7, 8,	// Temperature 7
		TIN8, 8,	// Temperature 8
		TIN9, 8,	// Temperature 9
		Offset (0x10),
		FAN0, 16,	// Fan Speed 0
		Offset (0x30),
		LIDS, 1,	// Lid Switch State
		PBTN, 1,	// Power Button Pressed
		WPDI, 1,	// Write Protect Disabled
		RECK, 1,	// Keyboard Initiated Recovery
		RECD, 1,	// Dedicated Recovery Mode
		Offset (0x40),
		BTVO, 32,	// Battery Present Voltage
		BTPR, 32,	// Battery Present Rate
		BTRA, 32,	// Battery Remaining Capacity
		ACEX, 1,	// AC Present
		BTEX, 1,	// Battery Present
		BFDC, 1,	// Battery Discharging
		BFCG, 1,	// Battery Charging
		BFCR, 1,	// Battery Level Critical
		Offset (0x50),
		BTDA, 32,	// Battery Design Capacity
		BTDV, 32,	// Battery Design Voltage
		BTDF, 32,	// Battery Last Full Charge Capacity
		BTCC, 32,	// Battery Cycle Count
		BMFG, 64,	// Battery Manufacturer String
		BMOD, 64,	// Battery Model String
		BSER, 64,	// Battery Serial String
		BTYP, 64,	// Battery Type String
		Offset (0x80),
		ALS0, 16,	// ALS reading 0 in lux
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

	Method (_CRS, 0, NotSerialized)
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
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
		\PNOT ()
	}

	// AC Disconnected
	Method (_Q05, 0, NotSerialized)
	{
		Store ("EC: AC DISCONNECTED", Debug)
		Store (ACEX, \PWRS)
		Notify (AC, 0x80)
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
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
		If (CondRefOf (\_TZ.THRT, Local0)) {
			\_TZ.THRT (1)
		}
	}

	// Throttle Stop
	Method (_Q13, 0, NotSerialized)
	{
		Store ("EC: THROTTLE STOP", Debug)
		If (CondRefOf (\_TZ.THRT, Local0)) {
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
		Divide (ToInteger (Arg1), 10, Local0, Local1)

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
		Divide (ToInteger (Arg1), 10, Local0, Local1)

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
			If (CondRefOf (\_SB.DPTF.TEVT, Local1)) {
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

#ifdef EC_ENABLE_ALS_DEVICE
	#include "als.asl"
#endif

#ifdef EC_ENABLE_PD_MCU_DEVICE
	#include "pd.asl"
#endif
}
