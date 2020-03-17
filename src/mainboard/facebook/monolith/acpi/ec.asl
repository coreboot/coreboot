/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * We only use the ERAM region to retrieve the CPU temperature. Otherwise the EC is not enabled
 * The _GPE is pointing to the correct pin but the EC events are not enabled in coreboot or the
 * EC.
 */

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, GPE0_DW0_22)

	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}

	Method (_REG, 2, NotSerialized)
	{
	}

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xFF)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x00),
		CPUT,   8,
	}

	Method (TSRD, 1, Serialized)
	{
		/* Prevent iasl remarks about unused parameters */
		Store(Arg0, Local0)
		Store(Local0, Arg0)
		Return (\_SB.DPTF.CTOK (CPUT))
	}

	/* Set Aux Trip Point 0 */
	Method (PAT0, 2, Serialized)
	{
		/* Prevent iasl remarks about unused parameters */
		Store(Arg0, Local0)
		Store(Local0, Arg0)
		Store(Arg1, Local0)
		Store(Local0, Arg1)
	}

	/* Set Aux Trip Point 1 */
	Method (PAT1, 2, Serialized)
	{
		/* Prevent iasl remarks about unused parameters */
		Store(Arg0, Local0)
		Store(Local0, Arg0)
		Store(Arg1, Local0)
		Store(Local0, Arg1)
	}

	/* Disable Aux Trip Point */
	Method (PATD, 1, Serialized)
	{
		/* Prevent iasl remarks about unused parameters */
		Store(Arg0, Local0)
		Store(Local0, Arg0)
	}
}
