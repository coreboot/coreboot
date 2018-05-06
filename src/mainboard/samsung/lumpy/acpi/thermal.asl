/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

// Thermal Zone

#define HAVE_THERMALZONE
Scope (\_TZ)
{
	ThermalZone (THRM)
	{
		Name (_TC1, 0x02)
		Name (_TC2, 0x05)

		// Ignore critical temps for the first few reads
		// at boot to prevent unexpected shutdown
		Name (IRDC, 4)
		Name (CRDC, 0)

		// Thermal zone polling frequency: 10 seconds
		Name (_TZP, 100)

		// Thermal sampling period for passive cooling: 2 seconds
		Name (_TSP, 20)

		// Convert from Degrees C to 1/10 Kelvin for ACPI
		Method (CTOK, 1) {
			// 10th of Degrees C
			Multiply (Arg0, 10, Local0)

			// Convert to Kelvin
			Add (Local0, 2732, Local0)

			Return (Local0)
		}

		// Threshold for OS to shutdown
		Method (_CRT, 0, Serialized)
		{
			Return (CTOK (\TCRT))
		}

		// Threshold for passive cooling
		Method (_PSV, 0, Serialized)
		{
			Return (CTOK (\TPSV))
		}

		// Processors used for passive cooling
		Method (_PSL, 0, Serialized)
		{
			Return (\PPKG ())
		}

		Method (_TMP, 0, Serialized)
		{
			// Get CPU Temperature from the Embedded Controller
			Store (\_SB.PCI0.LPCB.EC0.CPUT, Local0)

			// Re-read from EC if the temperature is very high to
			// avoid OS shutdown if we got a bad reading.
			If (LGreaterEqual (Local0, \TCRT)) {
				Store (\_SB.PCI0.LPCB.EC0.CPUT, Local0)
				If (LGreaterEqual (Local0, \TCRT)) {
					// Check if this is an early read
					If (LLess (CRDC, IRDC)) {
						Store (0, Local0)
					}
				}
			}

			// Keep track of first few reads by the OS
			If (LLess (CRDC, IRDC)) {
				Increment (CRDC)
			}

			// Invalid reading, ensure fan is spinning
			If (LGreaterEqual (Local0, 0x80)) {
				Return (CTOK (\F4ON))
			}

			Return (CTOK (Local0))
		}

		Method (_AC0) {
			If (LLessEqual (\FLVL, 0)) {
				Return (CTOK (\F0OF))
			} Else {
				Return (CTOK (\F0ON))
			}
		}

		Method (_AC1) {
			If (LLessEqual (\FLVL, 1)) {
				Return (CTOK (\F1OF))
			} Else {
				Return (CTOK (\F1ON))
			}
		}

		Method (_AC2) {
			If (LLessEqual (\FLVL, 2)) {
				Return (CTOK (\F2OF))
			} Else {
				Return (CTOK (\F2ON))
			}
		}

		Method (_AC3) {
			If (LLessEqual (\FLVL, 3)) {
				Return (CTOK (\F3OF))
			} Else {
				Return (CTOK (\F3ON))
			}
		}

		Method (_AC4) {
			If (LLessEqual (\FLVL, 4)) {
				Return (CTOK (\F4OF))
			} Else {
				Return (CTOK (\F4ON))
			}
		}

		Name (_AL0, Package () { \_SB.PCI0.LPCB.EC0.FAN0 })
		Name (_AL1, Package () { \_SB.PCI0.LPCB.EC0.FAN1 })
		Name (_AL2, Package () { \_SB.PCI0.LPCB.EC0.FAN2 })
		Name (_AL3, Package () { \_SB.PCI0.LPCB.EC0.FAN3 })
		Name (_AL4, Package () { \_SB.PCI0.LPCB.EC0.FAN4 })
	}
}
