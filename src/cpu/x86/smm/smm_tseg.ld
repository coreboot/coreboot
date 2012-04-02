/* Maximum number of CPUs/cores */
CPUS = 16;

SECTIONS
{
	/* This is the actual SMM handler.
	 *
	 * We just put code, rodata, data and bss all in a row.
	 */
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
		*(.data.rel.ro.*)

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

	/* We are using the TSEG interleaved to stuff the SMM handlers
	 * for all CPU cores in there. The jump table redirects the execution
	 * to the actual SMM handler
	 */
	. = 0x8000 - (( CPUS - 1) * 0x400);
	.jumptable : {
		*(.jumptable)
	}

	/* Data used in early SMM TSEG handler. */
	. = 0x8400;
	.earlydata : {
		*(.earlydata)
	}

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
