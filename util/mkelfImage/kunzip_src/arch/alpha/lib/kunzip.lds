PAGE_SIZE = 65536;
BASIC_ALIGN = 8;
OUTPUT_FORMAT("elf64-alpha")
ENTRY(__start)
SECTIONS
{
	. = PAGE_SIZE;
	_start = .;
	/*
	 * First we place the code and read only data (typically const declared).
	 * This get placed in rom.
	 */
	.text : {
		_text = .;
		*(.text)
		_etext = .;
		_rodata = .;
		*(.rodata);
		_erodata = .;
	}
	/* Global data */
	.data : {
		_data = .;
		*(.data)
		CONSTRUCTORS
		*(.got)
		*(.sdata)
		_edata = .;
	}

	/* Important align _bss so bss may be zeroed with quadword access */
	. = ALIGN(BASIC_ALIGN);
	.bss : {
		_bss = .;
		*(.sbss)
		*(.scommon)
		*(.bss)
		*(COMMON)
		*(.heap)
		*(.stack)
		/* Important align _ebss so bss may be zeroed with quadword access */
		. = ALIGN(BASIC_ALIGN);
		_ebss = .;
	}
	_end = .;

	/DISCARD/ : {
		*(*)
	}
}
