/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
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

Name (PBRS, ResourceTemplate ()
{
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
		0x0000, 0x0000, 0x00FF, 0x0000, 0x0100, ,, )

	/* System IO */
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0, 0x0, 0xffff, 0x0000, 0x10000, ,,, TypeStatic)
	IO (Decode16, 0x0CF8, 0x0CF8, 0x08, 0x08, )

	/* Video RAM */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
		0x00000000, 0x000A0000, 0x000BFFFF,
		0x00000000, 0x00020000, ,,, AddressRangeMemory, TypeStatic)

	/* Video ROM */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
		0x00000000, 0x000C0000, 0x000C7FFF,
		0x00000000, 0x00008000, ,,, AddressRangeMemory, TypeStatic)

	/* Option ROMs ? */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
		0x00000000, 0x000C8000, 0x000DFFFF,
		0x00000000, 0x00018000, ,,, AddressRangeMemory, TypeStatic)

	/* Top Of Lowmemory to IOAPIC */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
		0x00000000, 0x00000000, 0xFEBFFFFF,
		0x00000000, IO_APIC_ADDR, ,, _Y08, AddressRangeMemory, TypeStatic)
})


Method (_CRS, 0, NotSerialized)
{

	/* Top Of Lowmemory to IOAPIC */
	CreateDWordField (PBRS, \_SB.PCI0._Y08._MIN, MEML)
	CreateDWordField (PBRS, \_SB.PCI0._Y08._MAX, MEMH)
	CreateDWordField (PBRS, \_SB.PCI0._Y08._LEN, LENM)
	And (\_SB.PCI0.TOLM, 0xF800, Local1)
	ShiftRight (Local1, 0x04, Local1)
	ShiftLeft (Local1, 0x14, MEML)
	Subtract (IO_APIC_ADDR, 0x01, MEMH)
	Subtract (IO_APIC_ADDR, MEML, LENM)

	Return (PBRS)
}

Method (_STA, 0, NotSerialized)
{
	Return (0x0F)
}

