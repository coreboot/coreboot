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
/*
 INCLUDE ldoptions
 */
MEMORY { .sram : ORIGIN = 0x02023400, LENGTH = 0x3800 }

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)

SECTIONS
{
	. = ROMSTAGE_BASE;

	/*
	.rom . : {
		_rom = .;
		*(.rom.text);
		*(.rom.data);
		*(.rodata);
		*(.rodata.*);
		*(.rom.data.*);
		. = ALIGN(16);
		_erom = .;
	}
	*/

	/* First we place the code and read only data (typically const declared).
	 * This could theoretically be placed in rom.
	 */
	.text : {
		_text = .;
		*(.text);
		*(.text.*);
		. = ALIGN(4);
		_etext = .;
	} >.sram

	.rodata : {
		_rodata = .;
		. = ALIGN(4);
		cpu_drivers = . ;
		*(.rodata.cpu_driver)
		ecpu_drivers = . ;
		*(.rodata)
		*(.rodata.*)
		/* kevinh/Ispiri - Added an align, because the objcopy tool
		 * incorrectly converts sections that are not long word aligned.
		 */
		 . = ALIGN(4);

		_erodata = .;
	} >.sram
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
	} >.sram

	__image_copy_end = .;

	/* bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 * crt0.S fills between _bss and _ebss with zeroes.
	 */
	.bss . : {
		. = ALIGN(4);
		_bss = .;
		*(.bss)
		*(.sbss)
		*(COMMON)
	} >.sram
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
