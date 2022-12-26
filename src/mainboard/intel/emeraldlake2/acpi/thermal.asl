/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

External (\PPKG, MethodObj)

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
			Local0 = Arg0 * 10

			// Convert to Kelvin
			Local0 += 2732

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
			Local0 = \_SB.PCI0.LPCB.SIO.ENVC.TIN3

			// Check for invalid readings
			If ((Local0 == 255) || (Local0 == 0)) {
				Return (CTOK (\F2ON))
			}

			// PECI raw value is an offset from Tj_max
			Local1 = 255 - Local0

			// Handle values greater than Tj_max
			If (Local1 >= \TMAX) {
				Return (CTOK (\TMAX))
			}

			// Subtract from Tj_max to get temperature
			Local0 = \TMAX - Local1
			Return (CTOK (Local0))
		}

		Method (_AC0) {
			If (\FLVL <= 0) {
				Return (CTOK (\F0OF))
			} Else {
				Return (CTOK (\F0ON))
			}
		}

		Method (_AC1) {
			If (\FLVL <= 1) {
				Return (CTOK (\F1OF))
			} Else {
				Return (CTOK (\F1ON))
			}
		}

		Method (_AC2) {
			If (\FLVL <= 2) {
				Return (CTOK (\F2OF))
			} Else {
				Return (CTOK (\F2ON))
			}
		}

		Method (_AC3) {
			If (\FLVL <= 3) {
				Return (CTOK (\F3OF))
			} Else {
				Return (CTOK (\F3ON))
			}
		}

		Method (_AC4) {
			If (\FLVL <= 4) {
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
				If (\FLVL <= 0) {
					Return (1)
				} Else {
					Return (Zero)
				}
			}
			Method (_ON)  {
				\FLVL = 0
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F0PW
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				\FLVL = 1
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F1PW
				Notify (\_TZ.THRM, 0x81)
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
				\FLVL = 1
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F1PW
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				\FLVL = 2
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F2PW
				Notify (\_TZ.THRM, 0x81)
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
				\FLVL = 2
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F2PW
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				\FLVL = 3
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F3PW
				Notify (\_TZ.THRM, 0x81)
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
				\FLVL = 3
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F3PW
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				\FLVL = 4
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F4PW
				Notify (\_TZ.THRM, 0x81)
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
				\FLVL = 4
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F4PW
				Notify (\_TZ.THRM, 0x81)
			}
			Method (_OFF) {
				\FLVL = 4
				\_SB.PCI0.LPCB.SIO.ENVC.F3PS = \F4PW
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
