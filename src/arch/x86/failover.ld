/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

ENTRY(reset_vector)

MEMORY {
	rom : ORIGIN = 0xffff0000, LENGTH = 64K
}

TARGET(binary)
SECTIONS
{
	/* Symbol ap_sipi_vector must be aligned to 4kB to start AP CPUs
	 * with Startup IPI message without RAM. Align .rom to next 4 byte
	 * boundary anyway, so no pad byte appears between _rom and _start.
	 */
	.bogus ROMLOC_MIN : {
		. = CONFIG_SIPI_VECTOR_IN_ROM ?	ALIGN(4096) : ALIGN(4);
		ROMLOC = .;
	} >rom = 0xff

	/* This section might be better named .setup */
	.rom ROMLOC : {
		_rom = .;
		ap_sipi_vector = .;
		*(.rom.text);
		*(.rom.data);
		*(.rom.data.*);
		*(.text);
		*(.text.*);
		*(.rodata);
		*(.rodata.*);
		_erom = .;
	} >rom = 0xff

	/* Allocation reserves extra 16 bytes here. Alignment requirements
	 * may cause the total size of a section to change when the start
	 * address gets applied.
	 */
	ROMLOC_MIN = 0xffffff00 - (_erom - _rom + 16) -
		(CONFIG_SIPI_VECTOR_IN_ROM ? 4096 : 0);

	/* Post-check proper SIPI vector. */
	_bogus = ASSERT(!CONFIG_SIPI_VECTOR_IN_ROM || ((ap_sipi_vector & 0x0fff) == 0x0),
		"Bad SIPI vector alignment");
	_bogus = ASSERT(!CONFIG_SIPI_VECTOR_IN_ROM || (ap_sipi_vector == CONFIG_AP_SIPI_VECTOR),
		"Address mismatch on AP_SIPI_VECTOR");

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.eh_frame)
		*(.iplt)
		*(.rel.*)
		*(.igot.*)
	}
}
