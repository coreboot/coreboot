/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

External (\PPKG, MethodObj)

#define HAVE_THERMALZONE
Scope (\_TZ)
{
	ThermalZone (THRM)
	{
		Name (_TC1, 0x02)
		Name (_TC2, 0x03)

		// Thermal zone polling frequency: 10 seconds
		Name (_TZP, 100)

		// Thermal sampling period for passive cooling: 10 seconds
		Name (_TSP, 100)

		// Convert from Degrees C to 1/10 Kelvin for ACPI
		Method (CTOK, 1)
		{
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
			// Returns Higher of the two readings for CPU & VGA Temperature
			If (\_SB.PCI0.LPCB.EC0.TMP2 > \_SB.PCI0.LPCB.EC0.TMP1)
			{
				// CPU high temperature
				Local0 = \_SB.PCI0.LPCB.EC0.TMP2
			}
			Else
			{
				// VGA high temperature
				Local0 = \_SB.PCI0.LPCB.EC0.TMP1
			}

			// If temp less 35 or great then 115, set default 35
			If ((Local0 < 35) | (Local0 > 115))
			{
				Local0 = 35
			}

			Return (CTOK (Local0))
		}
	}
}
