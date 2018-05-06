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
			// Get CPU Temperature from PECI via SuperIO TMPIN3
			Store (\_SB.PCI0.LPCB.SIO.ENVC.TIN3, Local0)

			// Check for invalid readings
			If (LOr (LEqual (Local0, 255), LEqual (Local0, 0))) {
				Return (CTOK (\F2ON))
			}

			// PECI raw value is an offset from Tj_max
			Subtract (255, Local0, Local1)

			// Handle values greater than Tj_max
			If (LGreaterEqual (Local1, \TMAX)) {
				Return (CTOK (\TMAX))
			}

			// Subtract from Tj_max to get temperature
			Subtract (\TMAX, Local1, Local0)
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

		Name (_AL0, Package () { FAN0 })
		Name (_AL1, Package () { FAN1 })
		Name (_AL2, Package () { FAN2 })
		Name (_AL3, Package () { FAN3 })
		Name (_AL4, Package () { FAN4 })

		PowerResource (FNP0, 0, 0)
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
				Store (\F0PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (1, \FLVL)
				Store (\F1PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
		}

		PowerResource (FNP1, 0, 0)
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
				Store (\F1PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (2, \FLVL)
				Store (\F2PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
		}

		PowerResource (FNP2, 0, 0)
		{
			Method (_STA) {
				If (LLessEqual (\FLVL, 2)) {
					Return (One)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				Store (2, \FLVL)
				Store (\F2PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (3, \FLVL)
				Store (\F3PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
		}

		PowerResource (FNP3, 0, 0)
		{
			Method (_STA) {
				If (LLessEqual (\FLVL, 3)) {
					Return (One)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				Store (3, \FLVL)
				Store (\F3PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (4, \FLVL)
				Store (\F4PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
		}

		PowerResource (FNP4, 0, 0)
		{
			Method (_STA) {
				If (LLessEqual (\FLVL, 4)) {
					Return (One)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				Store (4, \FLVL)
				Store (\F4PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (4, \FLVL)
				Store (\F4PW, \_SB.PCI0.LPCB.SIO.ENVC.F3PS)
				Notify (\_TZ.THRM, 0x81)
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
	}
}
