/*
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

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

PHDRS
{
	to_load PT_LOAD;
}

ENTRY(lp0_resume)
SECTIONS
{
	. = 0x40020000 - 0x240;

	/*
	 * The lp0 blob header is built as a static data structure and put
	 * in the .header section.
	 */
	.header_start = .;
	.header . : {
		*(.header);
	} : to_load = 0xff
	.header_end = .;

	. = 0x40020000;

	/* The actual lp0 blob code. */
	.data_start = .;
	.data . : {
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
		. = ALIGN(16);
	}
	.data_end = .;

	/* Some values we need in the header. */
	blob_data = .data_start;
	blob_data_size = .data_end - .data_start;
	blob_total_size = .data_end - .header_start;

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.ARM.*)
	}
}
