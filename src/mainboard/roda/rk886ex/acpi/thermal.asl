/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

Scope (\_TZ)
{
	ThermalZone (THRM)
	{

		// FIXME these could/should be read from the
		// GNVS area, so they can be controlled by
		// coreboot
		Name(TC1V, 0x04)
		Name(TC2V, 0x03)
		Name(TSPV, 0x64)

		// At which temperature should the OS start
		// active cooling?
		Method (_AC0, 0, Serialized)
		{
			Return (0xf5c) // Value for Rocky
		}

		// Method (_AC1, 0, Serialized)
		// {
		//	Return (0xf5c)
		// }

		// Critical shutdown temperature
		Method (_CRT, 0, Serialized)
		{
			Return (0x0aac + 0x50) // FIXME
		}

		// CPU throttling start temperature
		Method (_PSV, 0, Serialized)
		{
			Return (0xaaf) // FIXME
		}

		// Get DTS Temperature
		Method (_TMP, 0, Serialized)
		{
			Return (0xaac) // FIXME
		}

		// Processors used for active cooling
		Method (_PSL, 0, Serialized)
		{
			If (MPEN) {
				Return (Package() {\_SB.CP00, \_SB.CP01})
			}
			Return (Package() {\_SB.CP00})
		}

		// TC1 value for passive cooling
		Method (_TC1, 0, Serialized)
		{
			Return (TC1V)
		}

		// TC2 value for passive cooling
		Method (_TC2, 0, Serialized)
		{
			Return (TC2V)
		}

		// Sampling period for passive cooling
		Method (_TSP, 0, Serialized)
		{
			Return (TSPV)
		}


	}
}
