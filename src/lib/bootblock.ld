/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

/* This file is included inside a SECTIONS block */

.bootblock . : {
	_program = .;
	_bootblock = .;
	*(.text._start);
	*(.text.stage_entry);
	KEEP(*(.id));
	*(.text);
	*(.text.*);
	*(.rodata);
	*(.rodata.*);
	*(.data);
	*(.data.*);
	*(.bss);
	*(.bss.*);
	*(.sbss);
	*(.sbss.*);
	_preram_cbmem_console = DEFINED(_preram_cbmem_console) ? _preram_cbmem_console : 0;
	_epreram_cbmem_console = DEFINED(_epreram_cbmem_console) ? _epreram_cbmem_console : 0;
	_ebootblock = .;
	_eprogram = .;
} : to_load = 0xff

/DISCARD/ : {
	*(.comment)
	*(.note)
	*(.comment.*)
	*(.note.*)
	*(.ARM.*)
	*(.MIPS.*)
}
