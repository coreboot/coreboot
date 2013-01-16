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

/*
 * FIXME: what exactly should these be? maybe defined on a per-CPU basis?
 * FIXME 2: Somehow linker didn't like CONFIG_SPL_MAX_SIZE and CONFIG_SPL_TEXT_BASE...
 */
/* MEMORY { .sram : ORIGIN = 0x02023400, LENGTH = 0x3800 } */
/*MEMORY { .sram : ORIGIN = 0x02023400, LENGTH = 0x10000 }*/

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)

SECTIONS
{
	. = CONFIG_ROMSTAGE_BASE;

	.romtext . : {
		_rom = .;
		_start = .;
		*(.text.startup);
		*(.text);
	}

	.romdata . : {
		*(.rodata);
		*(.machine_param);
		*(.data);
		. = ALIGN(8);
		_erom = .;
	}

	__image_copy_end = .;

	/* bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 * crt0.S fills between _bss and _ebss with zeroes.
	 */
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
