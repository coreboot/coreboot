/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/ioapic.h>

Name (_HID, EisaId ("PNP0A03"))
Name (_UID, 1)
Name (_ADR, 0x00000000)
Name (_BBN, 0)

/* The DRAM controller */
Device (MEMC)
{
	Name (_ADR, 0x00000003)

	OperationRegion (MEMB, PCI_Config, 0x00, 0xEF)
	Field (MEMB, DWordAcc, NoLock, Preserve) {

		/* DRAM Rank Ending Address */
		Offset (0x40),
		R0EA, 8,	/* Rank 0 Ending Address */
		R1EA, 8,	/* Rank 1 Ending Address */
		R2EA, 8,	/* Rank 2 Ending Address */
		R3EA, 8,	/* Rank 3 Ending Address */
	}

	/* Find the top of DRAM */
	Method (TOLM, 0) {
		/* Find the last occupied rank's end. */
		Store (R3EA, Local0)
		If (LEqual (Local0, Zero)) {
			Store (R2EA, Local0)
		}
		If (LEqual (Local0, Zero)) {
			Store (R1EA, Local0)
		}
		If (LEqual (Local0, Zero)) {
			Store (R0EA, Local0)
		}
		/* The granularity is 64M */
		ShiftLeft (Local0, 26, Local0)
		Return (Local0)
	}
}

Name (XCRS, ResourceTemplate () {
	/* All PCI busses */
	WordBusNumber (ResourceConsumer, MinNotFixed, MaxNotFixed, PosDecode,
		0x0000, 0x0000, 0x00ff, 0x0000, 0x0100,,,)

	/* IO-space, sans the PCI regs. */
	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000, 0x0000, 0x0CF7, 0x0000, 0x0CF8,
		,,, TypeStatic)
	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000, 0x0D00, 0xFFFF, 0x0000, 0xF300,
		,,, TypeStatic)

	/* The space from top of DRAM to IOAPIC */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
		/* This is a template that gets filled in _CRS() */
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,,,
		MEM0, AddressRangeMemory, TypeStatic)
})
Method (_CRS, 0) {

	/* MEM0 is from the top of RAM to IOAPIC */
	CreateDWordField (XCRS, \_SB.PCI0.MEM0._MIN, MEML)
	CreateDWordField (XCRS, \_SB.PCI0.MEM0._MAX, MEMH)
	CreateDWordField (XCRS, \_SB.PCI0.MEM0._LEN, LENM)
	Store (\_SB.PCI0.MEMC.TOLM, MEML)
	Subtract (IO_APIC_ADDR, 1, MEMH)
	Subtract (IO_APIC_ADDR, MEML, LENM)

	Return (XCRS);
}
