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

/* Global Variables */

Name(\PICM,0)			// IOAPIC/8259

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */


OperationRegion (GNVS, SystemMemory, 0xC0DEBABE, 0xFF)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	Offset (0x00),
	OSYS,	16,	// 0x00 Operating System
	SMIF,	 8,	// 0x02 SMI function
	Offset (0x10),
	MPEN,	 8,	// 0x10 Multi Processor Enable

}
