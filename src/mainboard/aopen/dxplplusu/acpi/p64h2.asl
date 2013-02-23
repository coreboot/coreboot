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

/* Interrupt routing for PCI 03:xx.x */

/* I/O APIC id 0x3 */
Device(PBIO)
{
	Name (_HID, "ACPI000A")
	Name (_ADR, 0x001c0000)
}

/* PCI-X bridge */
Device(P64B)
{
	Name (_ADR, 0x001d0000)
	Name (_PRT, Package() {
		Package() { 0x0002ffff, 0, 0, 24 }, /* PCI-X slot 1 */
		Package() { 0x0002ffff, 1, 0, 25 },
		Package() { 0x0002ffff, 2, 0, 26 },
		Package() { 0x0002ffff, 3, 0, 27 },
		Package() { 0x0003ffff, 0, 0, 28 }, /* PCI-X slot 2 */
		Package() { 0x0003ffff, 1, 0, 29 },
		Package() { 0x0003ffff, 2, 0, 30 },
		Package() { 0x0003ffff, 3, 0, 31 },
		Package() { 0x0004ffff, 0, 0, 32 }, /* On-board GbE */
	})

	Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
	OperationRegion (PBPC, PCI_Config, 0x00, 0xFF)
	Field (PBPC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x3E), BCRL,   8,  BCRH,   8
	}


	Device (ETH0)
	{
		Name (_ADR, 0x00040000)
		Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
	}
}


/* Interrupt routing for PCI 04:xx.x */

/* I/O APIC id 0x4 */
Device(PAIO)
{
	Name (_HID, "ACPI000A")
	Name (_ADR, 0x001e0000)
}

/* PCI-X bridge */
Device(P64A)
{
	Name (_ADR, 0x001f0000)
	Name (_PRT, Package() {
		Package() { 0x0002ffff, 0, 0, 48 }, /* PCI-X slot 3 */
		Package() { 0x0002ffff, 1, 0, 49 },
		Package() { 0x0002ffff, 2, 0, 50 },
		Package() { 0x0002ffff, 3, 0, 51 },
		Package() { 0x0003ffff, 0, 0, 52 }, /* PCI-X slot 4 */
		Package() { 0x0003ffff, 1, 0, 53 },
		Package() { 0x0003ffff, 2, 0, 54 },
		Package() { 0x0003ffff, 3, 0, 55 },
		Package() { 0x0004ffff, 0, 0, 54 }, /* On-board SCSI, GSI not 56 ? */
		Package() { 0x0004ffff, 1, 0, 55 }, /* On-board SCSI, GSI not 57  */
	})

	Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
	OperationRegion (PBPC, PCI_Config, 0x00, 0xFF)
	Field (PBPC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x3E), BCRL,   8,  BCRH,   8
	}

	#include "acpi/scsi.asl"
}


