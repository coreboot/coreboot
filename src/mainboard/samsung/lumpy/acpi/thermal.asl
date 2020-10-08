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
			// Get CPU Temperature from the Embedded Controller
			Local0 = \_SB.PCI0.LPCB.EC0.CPUT

			// Re-read from EC if the temperature is very high to
			// avoid OS shutdown if we got a bad reading.
			If (Local0 >= \TCRT) {
				Local0 = \_SB.PCI0.LPCB.EC0.CPUT
				If (Local0 >= \TCRT) {
					// Check if this is an early read
					If (CRDC < IRDC) {
						Local0 = 0
					}
				}
			}

			// Keep track of first few reads by the OS
			If (CRDC < IRDC) {
				CRDC++
			}

			// Invalid reading, ensure fan is spinning
			If (Local0 >= 0x80) {
				Return (CTOK (\F4ON))
			}

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

		Name (_AL0, Package () { \_SB.PCI0.LPCB.EC0.FAN0 })
		Name (_AL1, Package () { \_SB.PCI0.LPCB.EC0.FAN1 })
		Name (_AL2, Package () { \_SB.PCI0.LPCB.EC0.FAN2 })
		Name (_AL3, Package () { \_SB.PCI0.LPCB.EC0.FAN3 })
		Name (_AL4, Package () { \_SB.PCI0.LPCB.EC0.FAN4 })
	}
}
