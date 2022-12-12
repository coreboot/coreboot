/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

External (\PPKG, MethodObj)

#define HAVE_THERMALZONE
Scope (\_TZ)
{
	// Handler for throttle requests on this platform
	//  0 = Stop throttling
	//  1 = Start throttling
	Method (THRT, 1, Serialized)
	{
		If (Arg0 == 0) {
			/* Disable Power Limit */
			\_SB.PCI0.MCHC.CTLD ()
		} Else {
			/* Enable Power Limit */
			\_SB.PCI0.MCHC.CTLE (\F0PW)
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

		Method (TCHK, 0, Serialized)
		{
			// Get Temperature from TIN# set in NVS
			Local0 = \_SB.PCI0.LPCB.EC0.TINS (TMPS)

			// Check for sensor not calibrated
			If (Local0 == \_SB.PCI0.LPCB.EC0.TNCA) {
				Return (CTOK(0))
			}

			// Check for sensor not present
			If (Local0 == \_SB.PCI0.LPCB.EC0.TNPR) {
				Return (CTOK(0))
			}

			// Check for sensor not powered
			If (Local0 == \_SB.PCI0.LPCB.EC0.TNOP) {
				Return (CTOK(0))
			}

			// Check for sensor bad reading
			If (Local0 == \_SB.PCI0.LPCB.EC0.TBAD) {
				Return (CTOK(0))
			}

			// Adjust by offset to get Kelvin
			Local0 += \_SB.PCI0.LPCB.EC0.TOFS

			// Convert to 1/10 Kelvin
			Local0 *= 10
			Return (Local0)
		}

		Method (_TMP, 0, Serialized)
		{
			// Get temperature from EC in deci-kelvin
			Local0 = TCHK ()

			// Critical temperature in deci-kelvin
			Local1 = CTOK (\TCRT)

			If (Local0 >= Local1) {
				Printf ("CRITICAL TEMPERATURE: %o", Local0)

				// Wait 1 second for EC to re-poll
				Sleep (1000)

				// Re-read temperature from EC
				Local0 = TCHK ()

				Printf ("RE-READ TEMPERATURE: %o", Local0)
			}

			Return (Local0)
		}
	}
}
