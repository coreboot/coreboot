/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Maximum number of CPUs/cores */
CPUS = 4;
ENTRY(smm_handler_start);

SECTIONS
{
	/* This is the actual SMM handler.
	 *
	 * We just put code, rodata, data and bss all in a row.
	 */
	. = 0xa0000;
	.handler (.): {
		_program = .;
		/* Assembler stub */
		*(.handler)

		/* C code of the SMM handler */
		*(.text);
		*(.text.*);

		/* C read-only data of the SMM handler */
		. = ALIGN(16);
		*(.rodata)
		*(.rodata.*)

		/* C read-write data of the SMM handler */
		 . = ALIGN(4);
		*(.data)
		*(.data.*)

		/* C uninitialized data of the SMM handler */
		 . = ALIGN(4);
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(.sbss.*)

		/* What is this? (Something we don't need with -fno-common.) */
		*(COMMON)
		 . = ALIGN(4);
		_eprogram = .;
	}

	/* We are using the ASEG interleaved to stuff the SMM handlers
	 * for all CPU cores in there. The jump table redirects the execution
	 * to the actual SMM handler
	 */
	. = 0xa8000 - (( CPUS - 1) * 0x400);
	.jumptable : {
		KEEP(*(.jumptable));
	}

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
		*(.eh_frame)
		*(.debug_*)
	}
}
