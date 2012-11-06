/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 secunet Security Networks AG
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// Thermal Zone

Scope (\_TZ)
{
	/* degree Celcius to deci-Kelvin (ACPI temperature unit) */
	Method(C2dK, 1) {
		Add (2732, Multiply (Arg0, 10), Local0)
		Return (Local0)
	}

	ThermalZone (THRM)
	{

		// FIXME these could/should be read from the
		// GNVS area, so they can be controlled by
		// coreboot
		Name(TC1V, 0x04)
		Name(TC2V, 0x03)
		Name(TSPV, 0x96)

		// At which temperature should the OS start
		// active cooling?
		Method (_AC0, 0, Serialized)
		{
			Return (C2dK(120)) // Value for Rocky
		}

		// Critical shutdown temperature
		Method (_CRT, 0, Serialized)
		{
			Return (C2dK(155)) // Value for Rocky
		}

		// CPU throttling start temperature
		Method (_PSV, 0, Serialized)
		{
			Return (C2dK(105)) // Value for Rocky
		}

		// Get DTS Temperature
		Method (_TMP, 0, Serialized)
		{
			If (\_SB.PCI0.LPCB.EC0.ALRC) {
				Store(0, \_SB.PCI0.LPCB.EC0.ALRC)
				Return(_AC0())
			}

			If (\_SB.PCI0.LPCB.EC0.ALRL) {
				Store(0, THRO)
				Store(0, \_SB.PCI0.LPCB.EC0.ALRL)
			}

			If (\_SB.PCI0.LPCB.EC0.ALRH) {
				Store(1, THRO)
				Store(0, \_SB.PCI0.LPCB.EC0.ALRH)
			}

			/* vendor BIOS reports 0K if TCPU >= 128 deg C ?!? */
			Return (C2dK(\_SB.PCI0.LPCB.EC0.TCPU))
		}


		// Processors used for active cooling
		Method (_PSL, 0, Serialized)
		{
			If (MPEN) {
				Return (Package() {\_PR.CPU0, \_PR.CPU1})
			}
			Return (Package() {\_PR.CPU0})
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

