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

	/* 16KB for the heap at 64KB */
	. = 0x10000;
	.heap : {
		_heap = .;
		. = 0x4000;
		_eheap = .;
	}

	. = ALIGN(0x4000);
	.smm_c_handler : {
		_smm_c_handler_start = .;

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
		*(COMMON)
		 . = ALIGN(4);

		_smm_c_handler_end = .;
	}

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
