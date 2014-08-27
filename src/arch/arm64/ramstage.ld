/*
 *	Memory map:
 *
 *	CONFIG_RAMBASE		: text segment
 *				: rodata segment
 *				: data segment
 *				: bss segment
 *				: stack
 *				: heap
 */
/*
 * Copyright 2013 Google Inc.
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 */

/*
 *	Written by Johan Rydberg, based on work by Daniel Kahlin.
 *      Rewritten by Eric Biederman
 *  2005.12 yhlu add ramstage cross the vga font buffer handling
 */

/* We use ELF as output format. So that we can debug the code in some form. */
INCLUDE ldoptions

ENTRY(stage_entry)

PHDRS
{
	to_load PT_LOAD;
}

SECTIONS
{
	. = CONFIG_RAMSTAGE_BASE;

	.text : {
		_text = .;
		_start = .;
		*(.text.stage_entry);
		*(.text);
		*(.text.*);
		. = ALIGN(16);
		_etext = .;
	} : to_load

	.ctors : {
		. = ALIGN(0x100);
		__CTOR_LIST__ = .;
		KEEP(*(.ctors));
		LONG(0);
		LONG(0);
		__CTOR_END__ = .;
	}

	.rodata : {
		. = ALIGN(64);
		_rodata = .;
		console_drivers = .;
		KEEP(*(.rodata.console_drivers));
		econsole_drivers = . ;
		. = ALIGN(64);
		pci_drivers = . ;
		KEEP(*(.rodata.pci_driver));
		epci_drivers = . ;
		cpu_drivers = . ;
		KEEP(*(.rodata.cpu_driver));
		ecpu_drivers = . ;
		_bs_init_begin = .;
		KEEP(*(.bs_init));
		LONG(0);
		LONG(0);
		_bs_init_end = .;
		. = ALIGN(64);
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	}

	.data : {
		 . = ALIGN(64);
		_data = .;
		*(.data)
		*(.data.*)
		 . = ALIGN(64);
		_edata = .;
	}

	.bss : {
		 . = ALIGN(64);
		_bss = .;
		*(.bss)
		*(.bss.*)
		*(.sbss.*)
		*(COMMON)
		. = ALIGN(64);
		_ebss = .;
	}

	.heap : {
		_heap = .;
		/* Reserve CONFIG_HEAP_SIZE bytes for the heap */
		. = . + CONFIG_HEAP_SIZE ;
		. = ALIGN(64);
		_eheap = .;
	}

	/* arm64 chipsets need to define CONFIG_RAMSTAGE_STACK_(TOP|BOTTOM) */
	_stack = CONFIG_RAMSTAGE_STACK_BOTTOM;
	_estack = CONFIG_RAMSTAGE_STACK_TOP;

	/* The ram segment. This includes all memory used by the memory
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
