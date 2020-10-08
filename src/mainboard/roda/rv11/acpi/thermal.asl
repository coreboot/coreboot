/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

External (\PPKG, MethodObj)

#define HAVE_THERMALZONE
Scope (\_TZ)
{
	// Convert from Degrees C to 1/10 Kelvin for ACPI
	Method (CTOK, 1) {
		// 10th of Degrees C
		Local0 = Arg0 * 10

		// Convert to Kelvin
		Local0 += 2732

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
			Local0 = \_SB.PCI0.LPCB.EC0.CPUT

			If (Local0 >= 0x80)
			{
				Debug = "-----> CPU Temperature (INVALID): "
				Debug = Local0
				Return (CTOK (0))
			}

			Debug = "-----> CPU Temperature: "
			Debug = Local0

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
			Local0 = \_SB.PCI0.LPCB.EC0.LOCT

			If (Local0 >= 0x80)
			{
				Debug = "-----> LOC Temperature (INVALID): "
				Debug = Local0
				Return (CTOK (0))
			}

			Debug = "-----> LOC Temperature: "
			Debug = Local0

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
			Local0 = \_SB.PCI0.LPCB.EC0.OEMT

			If (Local0 >= 0x80)
			{
				Debug = "-----> OEM Temperature (INVALID): "
				Debug = Local0
				Return (CTOK (0))
			}

			Debug = "-----> OEM Temperature: "
			Debug = Local0

			Return (CTOK (Local0))
		}
	}
}
