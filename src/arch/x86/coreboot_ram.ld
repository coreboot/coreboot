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
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 */

/*
 *	Written by Johan Rydberg, based on work by Daniel Kahlin.
 *      Rewritten by Eric Biederman
 *  2005.12 yhlu add coreboot_ram cross the vga font buffer handling
 */

/* We use ELF as output format. So that we can debug the code in some form. */
INCLUDE ldoptions

ENTRY(_start)

SECTIONS
{
	. = CONFIG_RAMBASE;
	/* First we place the code and read only data (typically const declared).
	 * This could theoretically be placed in rom.
	 */
	.text : {
		_text = .;
		*(.textfirst);
		*(.text);
		*(.text.*);
		. = ALIGN(16);
		_etext = .;
	}

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

		/* If any changes are made to the driver start/symbols or the
		 * section names the equivalent changes need to made to
		 * rmodule.ld. */
		console_drivers = .;
		*(.rodata.console_drivers)
		econsole_drivers = . ;
		. = ALIGN(4);
		pci_drivers = . ;
		*(.rodata.pci_driver)
		epci_drivers = . ;
		cpu_drivers = . ;
		*(.rodata.cpu_driver)
		ecpu_drivers = . ;
		_bs_init_begin = .;
		*(.bs_init)
		_bs_init_end = .;

		*(.rodata)
		*(.rodata.*)
		/* kevinh/Ispiri - Added an align, because the objcopy tool
		 * incorrectly converts sections that are not long word aligned.
		 */
		 . = ALIGN(4);

		_erodata = .;
	}
	/* After the code we place initialized data (typically initialized
	 * global variables). This gets copied into ram by startup code.
	 * __data_start and __data_end shows where in ram this should be placed,
	 * whereas __data_loadstart and __data_loadend shows where in rom to
	 * copy from.
	 */
	.data : {
		_data = .;
		*(.data)
		_edata = .;
	}

	/* bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 * crt0.S fills between _bss and _ebss with zeroes.
	 */
	_bss = .;
	.bss . : {
		*(.bss)
		*(.sbss)
		*(COMMON)
	}
	_ebss = .;

        _heap = .;
        .heap . : {
                /* Reserve CONFIG_HEAP_SIZE bytes for the heap */
                . = CONFIG_HEAP_SIZE ;
                . = ALIGN(4);
        }
        _eheap = .;

	/* The ram segment. This includes all memory used by the memory
	 * resident copy of coreboot, except the tables that are produced on
	 * the fly, but including stack and heap.
	 */
	_ram_seg = _text;
	_eram_seg = _eheap;

	/* CONFIG_RAMTOP is the upper address of cached memory (among other
	 * things). We must not exceed beyond that address, there be dragons.
	 */
	_bogus = ASSERT( ( _eram_seg < (CONFIG_RAMTOP)) , "Please increase CONFIG_RAMTOP");

	/* Discard the sections we don't need/want */

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
