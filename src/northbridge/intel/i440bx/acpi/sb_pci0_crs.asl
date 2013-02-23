/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* i440bx Northbridge */
Device (NB)
{
	Name(_ADR, 0x00000000)
	OperationRegion(PCIC, PCI_Config, 0x00, 0x100)
}

Field (NB.PCIC, AnyAcc, NoLock, Preserve)
{
	Offset (0x67),	// DRB7
	DRB7,	8,
}

Method(TOM1, 0) {
	/* Multiply by 8MB to get TOM */
	Return(ShiftLeft(DRB7, 23))
}

Method(_CRS, 0) {
	Name(TMP, ResourceTemplate() {
		WordBusNumber(ResourceProducer, MinFixed, MaxFixed, PosDecode,
			0x0000,             // Granularity
			0x0000,             // Range Minimum
			0x00FF,             // Range Maximum
			0x0000,             // Translation Offset
			0x0100,             // Length
			,,
		)
		IO(Decode16, 0x0CF8, 0x0CF8, 1, 8)

		WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000,			/* address granularity */
			0x0000,			/* range minimum */
			0x0CF7,			/* range maximum */
			0x0000,			/* translation */
			0x0CF8			/* length */
		)

		WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000,			/* address granularity */
			0x0D00,			/* range minimum */
			0xFFFF,			/* range maximum */
			0x0000,			/* translation */
			0xF300			/* length */
		)

		/* memory space for PCI BARs below 4GB */
		Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
	})
	CreateDWordField(TMP, MMIO._BAS, MM1B)
	CreateDWordField(TMP, MMIO._LEN, MM1L)
	/*
	 * Declare memory between TOM1 and 4GB as available
	 * for PCI MMIO.
	 *
	 * Use ShiftLeft to avoid 64bit constant (for XP).
	 * This will work even if the OS does 32bit arithmetic, as
	 * 32bit (0x00000000 - TOM1) will wrap and give the same
	 * result as 64bit (0x100000000 - TOM1).
	 */
	Store(TOM1, MM1B)
	ShiftLeft(0x10000000, 4, Local0)
	Subtract(Local0, TOM1, Local0)
	Store(Local0, MM1L)

	Return(TMP)
}
