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

/* First we place the code and read only data (typically const declared).
 * This could theoretically be placed in rom.
 */
.text : {
	_program = .;
	_ramstage = .;
	_text = .;
	*(.text._start);
	*(.text.stage_entry);
	*(.text);
	*(.text.*);
	. = ALIGN(16);
	_etext = .;
} : to_load

#if IS_ENABLED(CONFIG_COVERAGE)
.ctors : {
	. = ALIGN(0x100);
	__CTOR_LIST__ = .;
	KEEP(*(.ctors));
	LONG(0);
	LONG(0);
	__CTOR_END__ = .;
}
#endif

/* TODO: align data sections to cache lines? (is that really useful?) */
.rodata : {
	_rodata = .;
	. = ALIGN(8);

	/* If any changes are made to the driver start/symbols or the
	 * section names the equivalent changes need to made to
	 * rmodule.ld. */
	_pci_drivers = . ;
	KEEP(*(.rodata.pci_driver));
	_epci_drivers = . ;
	_cpu_drivers = . ;
	KEEP(*(.rodata.cpu_driver));
	_ecpu_drivers = . ;
	_bs_init_begin = .;
	KEEP(*(.bs_init));
	LONG(0);
	LONG(0);
	_bs_init_end = .;
	_cbmem_init_hooks = .;
	KEEP(*(.rodata.cbmem_init_hooks));
	_ecbmem_init_hooks = .;

	*(.rodata)
	*(.rodata.*)
	/* kevinh/Ispiri - Added an align, because the objcopy tool
	 * incorrectly converts sections that are not long word aligned.
	 */
	 . = ALIGN(8);

	_erodata = .;
}

.data : {
	/* Move to different cache line to avoid false sharing with .rodata. */
	. = ALIGN(64);	/* May not be actual line size, not that important. */
	_data = .;
	*(.data)
	*(.data.*)
	_edata = .;
}

.bss . : {
	_bss = .;
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
	_ebss = .;
}

.heap . : {
	_heap = .;
	/* Reserve CONFIG_HEAP_SIZE bytes for the heap */
	. += CONFIG_HEAP_SIZE ;
	. = ALIGN(4);
	_eheap = .;
	_eramstage = .;
	_eprogram = .;
}

/* Discard the sections we don't need/want */

/DISCARD/ : {
	*(.comment)
	*(.note)
	*(.note.*)
}
