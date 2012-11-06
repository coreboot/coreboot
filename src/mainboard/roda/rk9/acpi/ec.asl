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
			Offset(0xb1),
		CPWR,   1,
		ACCH,   1, // AC connected (charger present)
		B1PR,   1, // battery 1 present
		B2PR,   1, // battery 2 present
		B1CH,   1, // battery 1 charged
		B2CH,   1, // battery 2 charged
			Offset(0xb2),
		B1CG,   1, // battery 1 charging
		B2CG,   1, // battery 2 charging
		B1LO,   1, // battery 1 low
		B2LO,   1, // battery 2 low
			Offset(0xb3),
		B1DW,  16, // battery 1 design capacity
		B1FW,  16, // battery 1 last full charge capacity
		B1DV,  16, // battery 1 design voltage
		B1PW,  16, // battery 1 present capacity
			Offset(0xbd),
		B1PV,  16, // battery 1 present voltage
			Offset(0xc1),
		B2DW,  16, // battery 2 design capacity
		B2FW,  16, // battery 2 last full charge capacity
		B2DV,  16, // battery 2 design voltage
		B2PW,  16, // battery 2 present capacity
			Offset(0xcb),
		B2PV,  16, // battery 2 present voltage
			Offset(0xcf),
		FDDI,   1, // floppy on lpt indicator?
		LIDC,   1, // LID switch
			Offset(0xd0),
		TCPU,   8, // T_CPU in deg Celcius
			Offset(0xd6),
		/* exact purpose of these three is guessed,
		   but it's something about cooling */
		ALRL,   1, // active cooling low limit
		ALRH,   1, // active cooling high limit
		ALRC,   1, // active cooling clear
			Offset(0xe8),
		B1RW,  16, // battery 1 remaining capacity
		B2RW,  16, // battery 2 remaining capacity
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

	Method (_Q30, 0)
	{
		Store("_Q30: AC In", Debug)
		Notify(ADP1, 0x80)	// Tell the Power Adapter
		PNOT()			// and the CPU and Battery
	}

	Method (_Q31, 0)
	{
		Store("_Q31: AC Out", Debug)
		Notify(ADP1, 0x80)	// Tell the Power Adapter
		PNOT()			// and the CPU and Battery
	}

	Method (_Q32, 0)
	{
		Store("_Q32: Bat1 In", Debug)
		Notify(BAT1, 0x81)
	}

	Method (_Q33, 0)
	{
		Store("_Q33: Bat1 Out", Debug)
		Notify(BAT1, 0x81)
	}

	Method (_Q34, 0)
	{
		Store("_Q34: Bat2 In", Debug)
		Notify(BAT2, 0x81)
	}

	Method (_Q35, 0)
	{
		Store("_Q35: Bat2 Out", Debug)
		Notify(BAT2, 0x81)
	}

	Method (_Q36, 0)
	{
		Store("_Q36: Bat1 Low Power", Debug)
		Notify(BAT1, 0x80)
	}

	Method (_Q37, 0)
	{
		Store("_Q37: Bat1 Full Charge", Debug)
		Notify(BAT1, 0x80)
	}

	Method (_Q38, 0)
	{
		Store("_Q38: Bat2 Low Power", Debug)
		Notify(BAT2, 0x80)
	}

	Method (_Q39, 0)
	{
		Store("_Q39: Bat2 Full Charge", Debug)
		Notify(BAT2, 0x80)
	}

	Method (_Q40, 0)
	{
		Store("_Q40: LID Open/Close", Debug)
		Notify(LID0, 0x80)
	}

	Method (_Q41, 0)
	{
		Store("_Q41: Floppy on Parallel Port: Call the Museum!", Debug)
	}

	Method (_Q50, 0)
	{
		Store("_Q50: Processor is hot", Debug)
		Notify(\_TZ.THRM, 0x80)
	}

	Method (_Q51, 0)
	{
		Store("_Q51: Processor is boiling", Debug)
		Notify(\_TZ.THRM, 0x80)
	}

	Method (_Q52, 0)
	{
		Store("_Q52: Processor is burning", Debug)
		Notify(\_TZ.THRM, 0x80)
	}

}
