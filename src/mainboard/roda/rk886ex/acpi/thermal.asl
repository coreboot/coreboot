/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
			Return (Add (0x0aac, 0x50)) // FIXME
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
				Return (Package() {\_PR.CP00, \_PR.CP01})
			}
			Return (Package() {\_PR.CP00})
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
