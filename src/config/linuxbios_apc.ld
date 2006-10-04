/*
 *	Memory map:
 *
 *	DCACHE_RAM_BASE		
 *				: data segment
 *				: bss segment
 *				: heap
 *				: stack
 */
/*
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 */

/*
 *	Written by Johan Rydberg, based on work by Daniel Kahlin.
 *      Rewritten by Eric Biederman
 *  2005.12 yhlu add linuxbios_ram cross the vga font buffer handling
 *  2006.05 yhlu tailed it to use it for AP code in cache
 */
/*
 *	We use ELF as output format. So that we can
 *	debug the code in some form. 
 */
INCLUDE ldoptions

ENTRY(_start)

SECTIONS
{
	. = DCACHE_RAM_BASE;
	/*
	 * First we place the code and read only data (typically const declared).
	 * This get placed in rom.
	 */
	.text : {
		_text = .;
		*(.text);
		*(.text.*);
		. = ALIGN(16);
		_etext = .;
	}
	.rodata : {
		_rodata = .;
		. = ALIGN(4);
		*(.rodata)
		*(.rodata.*)
		 . = ALIGN(4);
		_erodata = .;
	}	
	/*
	 * After the code we place initialized data (typically initialized
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
	/*
	 * bss does not contain data, it is just a space that should be zero
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
	_end = .;
	. = ALIGN(0x1000);
	_stack = .;
	.stack . : {
		. = 0x4000;
	}
	_estack = .;
        _heap = .;
        .heap . : {
                . = ALIGN(4);
        }
        _eheap = .;
	/* The ram segment
 	 * This is all address of the memory resident copy of linuxBIOS.
	 */
	_ram_seg = _text; 
	_eram_seg = _eheap;

	_bogus = ASSERT( ( _eram_seg <= ((DCACHE_RAM_BASE + DCACHE_RAM_SIZE - DCACHE_RAM_GLOBAL_VAR_SIZE))) , "linuxbios_apc is too big");

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
