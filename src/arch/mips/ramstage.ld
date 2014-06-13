/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * Based on src/arch/arm/ramstage.ld:
 *   Written by Johan Rydberg, based on work by Daniel Kahlin.
 *   Rewritten by Eric Biederman
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

SECTIONS
{
	. = CONFIG_SYS_SDRAM_BASE;

	.text : {
		_text = .;
		_start = .;
		*(.text.stage_entry.mips);
		*(.text);
		*(.text.*);
		. = ALIGN(16);
		_etext = .;
	} : to_load

	.ctors : {
		. = ALIGN(0x100);
		__CTOR_LIST__ = .;
		*(.ctors);
		LONG(0);
		__CTOR_END__ = .;
	}

	.rodata : {
		_rodata = .;
		. = ALIGN(4);
		console_drivers = .;
		KEEP(*(.rodata.console_drivers));
		econsole_drivers = . ;
		. = ALIGN(4);
		pci_drivers = . ;
		KEEP(*(.rodata.pci_driver));
		epci_drivers = . ;
		cpu_drivers = . ;
		KEEP(*(.rodata.cpu_driver));
		ecpu_drivers = . ;
		_bs_init_begin = .;
		KEEP(*(.bs_init));
		_bs_init_end = .;
		*(.rodata)
		*(.rodata.*)
		 . = ALIGN(4);
		_erodata = .;
	}

	.data : {
		_data = .;
		*(.data)
		_edata = .;
	}

	/* bss will be cleared by cbfs_load_stage */
	_bss = .;
	.bss . : {
		*(.bss)
		*(.sbss)
		*(COMMON)
	}
	_ebss = .;
	_end = .;

	/*
	 * coreboot from the perspective of the loader really "ends"
	 * here. Only symbols are placed after this.
	 */

        _heap = .;
        _eheap = . + CONFIG_HEAP_SIZE;

	_stack = CONFIG_STACK_BOTTOM;
	_estack = CONFIG_STACK_TOP;

	/*
	 * The ram segment. This includes all memory used by the memory
	 * resident copy of coreboot, except the tables that are produced on
	 * the fly, but including stack and heap.
	 */
	_ram_seg = _text;
	_eram_seg = _eheap;

	/* Discard the sections we don't need/want */

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
