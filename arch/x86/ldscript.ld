/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

/*
ENTRY(_start)
*/

BOOTBLOCK_SIZE = 20480;

TARGET(binary)
SECTIONS
{
	. = 0 - BOOTBLOCK_SIZE + 256; /* leave space for vpd */

	.stage0_1 . : {
		_stage0_1 = .;
		*(.text);
		*(.data);
		*(.bss);
		*(.rodata.*)
		*(.rodata)
		_estage0_1 = .;
	}
	/DISCARD/ : {
		*(.comment)
		*(.note.*)
		*(.note)
		*(.note.*)
	}
}

SECTIONS {
	_ROMTOP = 0xfffffff0;
	. = _ROMTOP;
	.resetvector . : {
		*(.reset)
	} 
}

