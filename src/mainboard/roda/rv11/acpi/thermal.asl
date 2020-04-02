/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Thermal Zone

External (\PPKG, MethodObj)

#define HAVE_THERMALZONE
Scope (\_TZ)
{
	// Convert from Degrees C to 1/10 Kelvin for ACPI
	Method (CTOK, 1) {
		// 10th of Degrees C
		Multiply (Arg0, 10, Local0)

		// Convert to Kelvin
		Add (Local0, 2732, Local0)

		Return (Local0)
	}

	ThermalZone (THRM)
	{
		Name (_TC1, 0x02)
		Name (_TC2, 0x05)

		// Thermal zone polling frequency: 10 seconds
		Name (_TZP, 100)

		// Thermal sampling period for passive cooling: 2 seconds
		Name (_TSP, 20)

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

		Method (_TMP, 0, NotSerialized)  // _TMP: Temperature
		{
			Store (\_SB.PCI0.LPCB.EC0.CPUT, Local0)

			If (Local0 >= 0x80)
			{
				Store ("-----> CPU Temperature (INVALID): ", Debug)
				Store (Local0, Debug)
				Return (CTOK (0))
			}

			Store ("-----> CPU Temperature: ", Debug)
			Store (Local0, Debug)

			Return (CTOK (Local0))
		}

	}

	ThermalZone (TZ00)
	{
		// Thermal zone polling frequency: 10 seconds
		Name (_TZP, 100)

		// Thermal sampling period for passive cooling: 2 seconds
		Name (_TSP, 20)

		// Threshold for OS to shutdown
		Method (_CRT, 0, Serialized)
		{
			Return (CTOK (106))
		}

		Method (_TMP, 0, NotSerialized)  // _TMP: Temperature
		{
			Store (\_SB.PCI0.LPCB.EC0.LOCT, Local0)

			If (Local0 >= 0x80)
			{
				Store ("-----> LOC Temperature (INVALID): ", Debug)
				Store (Local0, Debug)
				Return (CTOK (0))
			}

			Store ("-----> LOC Temperature: ", Debug)
			Store (Local0, Debug)

			Return (CTOK (Local0))
		}
	}

	ThermalZone (TZ01)
	{
		// Thermal zone polling frequency: 10 seconds
		Name (_TZP, 100)

		// Thermal sampling period for passive cooling: 2 seconds
		Name (_TSP, 20)

		// Threshold for OS to shutdown
		Method (_CRT, 0, Serialized)
		{
			Return (CTOK (106))
		}

		Method (_TMP, 0, NotSerialized)  // _TMP: Temperature
		{
			Store (\_SB.PCI0.LPCB.EC0.OEMT, Local0)

			If (Local0 >= 0x80)
			{
				Store ("-----> OEM Temperature (INVALID): ", Debug)
				Store (Local0, Debug)
				Return (CTOK (0))
			}

			Store ("-----> OEM Temperature: ", Debug)
			Store (Local0, Debug)

			Return (CTOK (Local0))
		}
	}
}
