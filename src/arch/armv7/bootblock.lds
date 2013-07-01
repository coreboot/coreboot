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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

PHDRS
{
	to_load PT_LOAD;
}

TARGET(binary)
SECTIONS
{
	ROMLOC = CONFIG_BOOTBLOCK_BASE;

	/* This section might be better named .setup */
	.rom ROMLOC : {
		_rom = .;
		*(.start);
		*(.id);
		*(.text);
		*(.text.*);
		*(.rom.text);
		*(.rom.data);
		*(.rom.data.*);
		*(.rodata.*);
		_erom = .;
	} : to_load = 0xff

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.ARM.*)
	}
}
