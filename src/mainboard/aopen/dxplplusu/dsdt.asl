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

#include <arch/ioapic.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x04,		// DSDT revision: ACPI v4.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20111103	// OEM revision
) {

Scope(\_SB)
{
	Device(PCI0) {
		Name (_HID, EISAID("PNP0A03"))
		Name (_ADR, 0x00)
		Name (_PRT, Package() {
			Package() { 0x001dffff, 0, 0, 16 },
			Package() { 0x001dffff, 1, 0, 19 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 23 },
			Package() { 0x001fffff, 0, 0, 18 },
			Package() { 0x001fffff, 1, 0, 17 },
		})

		#include "acpi/e7505_sec.asl"

		OperationRegion (I750, PCI_Config, 0x00, 0x0100)
		Field (I750, ByteAcc, NoLock, Preserve)
		{
			Offset (0xC4),
				TOLM,   16,	/* Top of Low Memory */
				RBAR,   16,	/* REMAP_BASE */
				RLAR,   16	/* REMAP_LIMIT */
		}
	}

	#include "acpi/e7505_pri.asl"


	Device (PWBT)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_PRW, Package () { 0x08, 0x05 })
	}

	Device (SLBT)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_PRW, Package () { 0x0B, 0x05 })
	}

	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Name (_PRW, Package () { 0x0B, 0x05 })
	}

}

Scope(\_SB.PCI0)
{

	Device(PCI1)
	{
		Name (_ADR, 0x00010000)
		Name (_PRT, Package() {
			Package() { 0x0000ffff, 0, 0, 16 },
			Package() { 0x0000ffff, 1, 0, 17 },
		})
	}

	Device(HLIB)
	{
		Name (_ADR, 0x00020000)
		Name (_PRT, Package() {
			Package() { 0x001dffff, 0, 0, 18 },
			Package() { 0x001dffff, 1, 0, 18 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 18 },
			Package() { 0x001fffff, 0, 0, 18 },
			Package() { 0x001fffff, 1, 0, 18 },
			Package() { 0x001fffff, 2, 0, 18 },
			Package() { 0x001fffff, 3, 0, 18 },
		})

		#include "acpi/p64h2.asl"
	}

	#include "acpi/i82801db.asl"
}

#include "acpi/power.asl"

}

