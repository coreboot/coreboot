/* SPDX-License-Identifier: GPL-2.0-only */

// Thermal Zone

Scope (\_TZ)
{
	ThermalZone (THRM)
	{
		// TODO These could/should be read from the
		// GNVS area, so they can be controlled by
		// coreboot
		Name(TC1V, 0x00)
		Name(TC2V, 0x0a)
		Name(TSPV, 0x32)


		// Convert from °C to 1/10 Kelvin
		Method(DEGR, 1, NotSerialized)
		{
			Local0 = Arg0
			// 10ths of degrees
			Local0 *= 10
			// 0°C is 273.15 K, we need to round it.
			Local0 += 2732
			Return(Local0)
		}

		// At which temperature should the OS start
		// active cooling?
		Method (_AC0, 0, Serialized)
		{
			Return (0xf5c) // Value for Rocky
		}

		// Critical shutdown temperature
		Method (_CRT, 0, Serialized)
		{
			Local0 = \_SB.PCI0.LPCB.EC0.CRTT
			Local0 = DEGR (Local0)
			Return(Local0)
		}

		// CPU throttling start temperature
		Method (_PSV, 0, Serialized)
		{
			Local0 = \_SB.PCI0.LPCB.EC0.CTRO
			Local0 = DEGR (Local0)
			Return(Local0)
		}

		// Get DTS Temperature
		Method (_TMP, 0, Serialized)
		{
			Local0 = \_SB.PCI0.LPCB.EC0.CTMP
			Local0 = DEGR (Local0)
			Return(Local0)
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
