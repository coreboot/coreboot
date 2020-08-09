/* SPDX-License-Identifier: GPL-2.0-only */

/* Maximum number of CPUs/cores */
CPUS = 4;

_ = ASSERT(CPUS >= CONFIG_MAX_CPUS, "The ASEG SMM code only supports up to 4 CPUS");

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
