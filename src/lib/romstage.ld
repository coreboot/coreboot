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
 * Foundation, Inc.
 */

/* This file is included inside a SECTIONS block */

.text . : {
	_program = .;
	_romstage = .;
	*(.text._start);
	*(.text.stage_entry);
	*(.text);
	*(.text.*);
} : to_load

.data . : {
	. = ALIGN(8);
	_cbmem_init_hooks = .;
	KEEP(*(.rodata.cbmem_init_hooks));
	_ecbmem_init_hooks = .;
	*(.rodata);
	*(.rodata.*);
	*(.data);
	*(.data.*);
	_preram_cbmem_console = DEFINED(_preram_cbmem_console) ? _preram_cbmem_console : 0;
	_epreram_cbmem_console = DEFINED(_epreram_cbmem_console) ? _epreram_cbmem_console : 0;
	. = ALIGN(8);
}

.bss . : {
	. = ALIGN(8);
	_bss = .;
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
	_ebss = .;
	_eromstage = .;
	_eprogram = .;
}

/* Discard the sections we don't need/want */
/DISCARD/ : {
	*(.comment)
	*(.note)
	*(.comment.*)
	*(.note.*)
	*(.eh_frame);
}
