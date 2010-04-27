
/* Maximum number of CPUs/cores */
CPUS = 4;

SECTIONS
{
	/* This is the actual SMM handler.
	 *
	 * We just put code, rodata, data and bss all in a row.
	 */
	. = 0xa0000;
	.handler (.): {
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

		/* C uninitialized data of the SMM handler */
		 . = ALIGN(4);
		*(.bss)
		*(.sbss)

		/* What is this? */
		*(COMMON)
		 . = ALIGN(4);
	}

	/* We are using the ASEG interleaved to stuff the SMM handlers
	 * for all CPU cores in there. The jump table redirects the execution
	 * to the actual SMM handler
	 */
	. = 0xa8000 - (( CPUS - 1) * 0x400);
	.jumptable : {
		*(.jumptable)
	}

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
