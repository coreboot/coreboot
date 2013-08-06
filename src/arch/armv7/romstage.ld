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
		. = ALIGN(16);
		_car_migrate_start = .;
		*(.car.migrate);
		_car_migrate_end = .;
		. = ALIGN(16);
		_erom = .;
	}

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

	.car.data . : {
		. = ALIGN(8);
		_car_data_start = .;
		*(.car.global_data);
		. = ALIGN(8);
		/* The cbmem_console section comes last to take advantage of
		 * a zero-sized array to hold the memconsole contents that
		 * grows to a bound of CONFIG_CONSOLE_CAR_BUFFER_SIZE. However,
		 * collisions within the cache-as-ram region cannot be
		 * statically checked because the cache-as-ram region usage is
		 * cpu/chipset dependent. */
		*(.car.cbmem_console);
		_car_data_end = .;
	}

	_end = .;

	/* TODO: check if we are running out of SRAM. Below check is not good
	 * enough though because SRAM has different size on different CPUs
	 * and not all SRAM is available to the romstage. On Exynos, some is
	 * used for BL1, the bootblock and the stack.
	 *
	 * _bogus = ASSERT((_end - _start + EXPECTED_CBMEM_CONSOLE_SIZE <= \
	 * 				0x54000), "SRAM area is too full");
	 */

	/* Discard the sections we don't need/want */
	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.eh_frame);
	}
}
