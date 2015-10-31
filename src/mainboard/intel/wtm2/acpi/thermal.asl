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
#include "../thermal.h"

Scope (\_TZ)
{
	ThermalZone (THRM)
	{
		Name (_TC1, 0x02)
		Name (_TC2, 0x05)

		// Thermal zone polling frequency: 0 seconds
		Name (_TZP, 0)

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
			Return (CTOK (30))
		}

		Method (_AC0) {
			If (LLessEqual (\FLVL, 0)) {
				Return (CTOK (FAN0_THRESHOLD_OFF))
			} Else {
				Return (CTOK (FAN0_THRESHOLD_ON))
			}
		}

		Method (_AC1) {
			If (LLessEqual (\FLVL, 1)) {
				Return (CTOK (FAN1_THRESHOLD_OFF))
			} Else {
				Return (CTOK (FAN1_THRESHOLD_ON))
			}
		}

		Method (_AC2) {
			If (LLessEqual (\FLVL, 2)) {
				Return (CTOK (FAN2_THRESHOLD_OFF))
			} Else {
				Return (CTOK (FAN2_THRESHOLD_ON))
			}
		}

		Method (_AC3) {
			If (LLessEqual (\FLVL, 3)) {
				Return (CTOK (FAN3_THRESHOLD_OFF))
			} Else {
				Return (CTOK (FAN3_THRESHOLD_ON))
			}
		}

		Method (_AC4) {
			If (LLessEqual (\FLVL, 4)) {
				Return (CTOK (FAN4_THRESHOLD_OFF))
			} Else {
				Return (CTOK (FAN4_THRESHOLD_ON))
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
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (1, \FLVL)
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
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (2, \FLVL)
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
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (3, \FLVL)
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
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (4, \FLVL)
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
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				Store (4, \FLVL)
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
