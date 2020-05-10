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
			Store (\_SB.PCI0.LPCB.EC0.CTMP, Local0)

			// Re-read from EC if the temperature is very high to
			// avoid OS shutdown if we got a bad reading.
			If (LGreaterEqual (Local0, \TCRT)) {
				Store (\_SB.PCI0.LPCB.EC0.CTMP, Local0)
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

			Return (CTOK (Local0))
		}

// The EC does all fan control. The is no Active Cooling Fan control (_ACx).

	}
}
