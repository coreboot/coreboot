/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/thermal.h>

External (\PPKG, MethodObj)

/* Thermal Zone */

Scope (\_TZ)
{
	ThermalZone (THRM)
	{
		/* Thermal constants for passive cooling */
		Name (_TC1, 0x02)
		Name (_TC2, 0x05)

		/* Thermal zone polling frequency: 10 seconds */
		Name (_TZP, 100)

		/* Thermal sampling period for passive cooling: 2 seconds */
		Name (_TSP, 20)

		/* Convert from Degrees C to 1/10 Kelvin for ACPI */
		Method (CTOK, 1) {
			/* 10th of Degrees C */
			Multiply (Arg0, 10, Local0)

			/* Convert to Kelvin */
			Local0 += 2732

			Return (Local0)
		}

		/* Threshold for OS to shutdown */
		Method (_CRT, 0, Serialized)
		{
			Return (CTOK (\TCRT))
		}

		/* Threshold for passive cooling */
		Method (_PSV, 0, Serialized)
		{
			Return (CTOK (\TPSV))
		}

		/* Processors used for passive cooling */
		Method (_PSL, 0, Serialized)
		{
			Return (\PPKG ())
		}

		Method (_TMP, 0, Serialized)
		{
			/* Get temperature from EC in deci-kelvin */
			Store (\_SB.PCI0.LPCB.EC0.TSRD (TMPS), Local0)

			/* Critical temperature in deci-kelvin */
			Store (CTOK (\TCRT), Local1)

			If (Local0 >= Local1) {
				Printf ("CRITICAL TEMPERATURE: %o", Local0)

				/* Wait 1 second for EC to re-poll */
				Sleep (1000)

				/* Re-read temperature from EC */
				Store (\_SB.PCI0.LPCB.EC0.TSRD (TMPS), Local0)

				Printf ("RE-READ TEMPERATURE: %o", Local0)
			}

			Return (Local0)
		}

		/* No active fan control (_ACx) on Kahlee */
	}
}
