OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)


ENTRY(startup_32)
SECTIONS
{
	. = 0x10000;
	_text = .;			/* Text and read-only data */
	.text . : {
		*(.text)
		*(.text.*)
	}  = 0x9090
	.rodata (.): {
		*(.rodata)
		*(.rodata.*)
	}
	_etext = .;			/* End of text section */
	.data (.): {			/* Data */
		*(.data)
		*(.data.*)
		*(.trailer)
	}
	_edata = .;			/* End of data section */
	_bss = .;			/* BSS */
	.bss (.): {
		*(.bss)
	}
	_end = . ;
	bss_sizex = _end - _bss;

	/DISCARD/ : {
		*(.comment)
		*(.note)
	}
}
