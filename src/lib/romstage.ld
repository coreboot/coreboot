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

.text . : {
	_program = .;
	_romstage = .;
	*(.text._start);
	*(.text.stage_entry);
	*(.text);
	*(.text.*);
} : to_load

.data . : {
	*(.rodata);
	*(.rodata.*);
	*(.data);
	*(.data.*);
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
