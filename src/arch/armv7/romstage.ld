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

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(stage_entry)

PHDRS
{
	to_load PT_LOAD;
}

SECTIONS
{
	/* TODO make this a configurable option (per chipset). */
	. = CONFIG_ROMSTAGE_BASE;

	.romtext . : {
		_rom = .;
		_start = .;
		*(.text.stage_entry.armv7);
		*(.text.startup);
		*(.text);
	} : to_load

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
