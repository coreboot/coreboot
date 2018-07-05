/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
	ThermalZone (CRIT)
	{
		// Thermal zone polling frequency: 5 seconds
		Name (_TZP, 50)

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

		Method (_TMP, 0, Serialized)
		{
			// Get CPU Temperature from TIN9/PECI via EC
			Store (\_SB.PCI0.LPCB.EC0.TIN9, Local0)

			// Check for sensor not calibrated
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNCA)) {
				Return (CTOK(0))
			}

			// Check for sensor not present
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNPR)) {
				Return (CTOK(0))
			}

			// Check for sensor not powered
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNOP)) {
				Return (CTOK(0))
			}

			// Check for sensor bad reading
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TBAD)) {
				Return (CTOK(0))
			}

			// Adjust by offset to get Kelvin
			Add (\_SB.PCI0.LPCB.EC0.TOFS, Local0, Local0)

			// Convert to 1/10 Kelvin
			Multiply (Local0, 10, Local0)
			Return (Local0)
		}
	}

	ThermalZone (THRM)
	{
		Name (_TC1, 0x02)
		Name (_TC2, 0x05)

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
			// Get Temperature from TIN# set in NVS
			Store (\_SB.PCI0.LPCB.EC0.TINS (TMPS), Local0)

			// Check for sensor not present
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNPR)) {
				Return (CTOK(0))
			}

			// Check for sensor not powered
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TNOP)) {
				Return (CTOK(0))
			}

			// Check for sensor bad reading
			If (LEqual (Local0, \_SB.PCI0.LPCB.EC0.TBAD)) {
				Return (CTOK(0))
			}

			// Adjust by offset to get Kelvin
			Add (\_SB.PCI0.LPCB.EC0.TOFS, Local0, Local0)

			// Convert to 1/10 Kelvin
			Multiply (Local0, 10, Local0)
			Return (Local0)
		}

		/* CTDP Down */
		Method (_AC0) {
			If (LLessEqual (\FLVL, 0)) {
				Return (CTOK (\F0OF))
			} Else {
				Return (CTOK (\F0ON))
			}
		}

		/* CTDP Nominal */
		Method (_AC1) {
			If (LLessEqual (\FLVL, 1)) {
				Return (CTOK (\F1OF))
			} Else {
				Return (CTOK (\F1ON))
			}
		}

		Name (_AL0, Package () { TDP0 })
		Name (_AL1, Package () { TDP1 })

		PowerResource (TNP0, 0, 0)
		{
			Method (_STA) {
				If (LLessEqual (\FLVL, 0)) {
					Return (One)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				Store (0, \FLVL)
				\_SB.PCI0.MCHC.STND ()
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (1, \FLVL)
				\_SB.PCI0.MCHC.STDN ()
				Notify (\_TZ.THRM, 0x81)
			}
		}

		PowerResource (TNP1, 0, 0)
		{
			Method (_STA) {
				If (LLessEqual (\FLVL, 1)) {
					Return (One)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				Store (1, \FLVL)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (1, \FLVL)
				Notify (\_TZ.THRM, 0x81)
			}
		}

		Device (TDP0)
		{
			Name (_HID, EISAID ("PNP0C0B"))
			Name (_UID, 0)
			Name (_PR0, Package () { TNP0 })
		}

		Device (TDP1)
		{
			Name (_HID, EISAID ("PNP0C0B"))
			Name (_UID, 1)
			Name (_PR0, Package () { TNP1 })
		}
	}
}
