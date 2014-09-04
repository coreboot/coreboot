/*
 *	Memory map:
 *
 *	CONFIG_VERSTAGE_BASE	: text segment
 *				: rodata segment
 *				: data segment
 *				: bss segment
 */

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
INCLUDE ldoptions

PHDRS
{
	to_load PT_LOAD;
}

ENTRY(stage_entry)

SECTIONS
{
	. = CONFIG_VERSTAGE_BASE;

	.romtext . : {
		_start = .;
		*(.text.stage_entry.arm);
		*(.text.startup);
		*(.text);
		*(.text.*);
	} : to_load

	.romdata . : {
		*(.rodata);
		*(.rodata.*);
		*(.data);
		*(.data.*);
		. = ALIGN(8);
	}

	/* bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 */
	.bss . : {
		. = ALIGN(8);
		_bss = .;
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(.sbss.*)
		_ebss = .;
	}

	_end = .;

	preram_cbmem_console = CONFIG_CONSOLE_PRERAM_BUFFER_BASE;

	/* Discard the sections we don't need/want */
	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.eh_frame);
	}
}
