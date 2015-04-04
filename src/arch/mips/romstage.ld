/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

OUTPUT_ARCH(mips)

ENTRY(stage_entry)

PHDRS
{
	to_load PT_LOAD;
}

preram_cbmem_console = CONFIG_CBMEM_CONSOLE_PRERAM_BASE;

SECTIONS
{
	. = CONFIG_ROMSTAGE_BASE;

	.romtext . : {
		_rom = .;
		_start = .;
		*(.text.stage_entry.mips);
		*(.text.startup);
		*(.text);
	} : to_load

	.romdata . : {
		*(.rodata);
		*(.data);
		. = ALIGN(16);
		_erom = .;
	}

	/* bss will be cleared by cbfs_load_stage */
	.bss . : {
		. = ALIGN(8);
		_bss = .;
		*(.bss)
		*(.sbss)
		*(COMMON)
	}

	_ebss = .;
	_end = .;

	/* Discard the sections we don't need/want */
	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.eh_frame);
	}
}
