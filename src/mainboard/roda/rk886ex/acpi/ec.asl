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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

Device(EC0)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 1)

	Name (_GPE, 23)	// GPI07 / GPE23 -> Runtime SCI
	Name (ECON, 0)
	Name (QEVT, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0x04), // Command interface
		CMCM,	  8,
		CMD1,	  8,
		CMD2,	  8,
		CMD3,	  8,
		Offset(0x18), // SMBus
		SMPR,	  8,
		SMST,	  8,
		SMAD,	  8,
		SMCM,	  8,
		Offset(0x1c),
		SMW0,	 16,
		//SMD0,	264,
		Offset(0x3d),
		SMAA,	  8,
		Offset(0x78), // GPIs
		P60S,	  1,
		P61S,	  1,
		P62S,	  1,
		P63S,	  1,
		P64S,	  1,
		P65S,	  1,
		P66S,	  1,
		P67S,	  1,
		Offset(0x83), // Thermal
		RTMP,	  8,
		TML0,	  8,
		TMH0,	  8,
		Offset(0x87),
		TMCR,	  8,
		Offset(0x89),
		TML1,	  8,
		TMH1,	  8
	}

	Method (_CRS, 0)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})

		Return (ECMD)
	}

	Method (_REG, 2)
	{
		// This method is needed by Windows XP/2000 for
		// EC initialization before a driver is loaded

		If (LEqual(Arg0, 0x03)) {
			Store (Arg1, ECON)
		}
	}

	// EC Query methods

	Method (_Q11, 0)
	{
		Store("_Q11: Fn-F8 (Sleep Button) pressed", Debug)
		Notify(SLPB, 0x80)
	}

	Method (_Q12, 0)
	{
		Store("_Q12: Fn-F9 (Display Switch) pressed", Debug)
		// Store(1, TLST)
		// HKDS(10)
	}

	Method (_Q30, 0)
	{
		Store("_Q30: AC In/Out", Debug)
		Notify(ADP1, 0x80)	// Tell the Power Adapter
		PNOT()			// and the CPU and Battery
	}

	Method (_Q31, 0)
	{
		Store("_Q31: LID Open/Close", Debug)
		Notify(LID0, 0x80)
	}

	Method (_Q32, 0)
	{
		Store("_Q32: Battery 1 In/Out", Debug)
		If (ECON) {
			Store (P62S, Local0)
			If (Not(Local0)) {
				Notify(BAT1, 0x80)
			}
		}
	}

	Method (_Q33, 0)
	{
		Store("_Q33: Battery 2 In/Out", Debug)
		If (ECON) {
			Store (P63S, Local0)
			If (Not(Local0)) {
				Notify(BAT2, 0x80)
			}
		}
	}

	Method (_Q34, 0)
	{
		Store("_Q34: LPT/FDD", Debug)
		// PHSS(0x70)
	}

	Method (_Q35, 0)
	{
		Store("_Q35: Processor is hot", Debug)
	}

	Method (_Q36, 0)
	{
		Store("_Q36: Thermal Warning", Debug)
	}

	Method (_Q37, 0)
	{
		Store("_Q37: PME", Debug)
	}

	Method (_Q38, 0)
	{
		Store("_Q38: Thermal", Debug)
	}

	Method (_Q39, 0)
	{
		Store("_Q39: Thermal", Debug)
	}

	// TODO Scope _SB devices for AC power, LID, Power button

}
