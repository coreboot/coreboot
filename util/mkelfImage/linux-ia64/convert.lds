OUTPUT_FORMAT("elf64-ia64-little")
OUTPUT_ARCH(ia64)

ENTRY(_start)
SECTIONS {
	/* . = 0; */
	. = 0x0000000004400000;
	.text : {
		_text = . ;
		*(.text)
		*(.rodaa)
		*(.rodata.*)
		_etext = . ;
	}
	. = ALIGN(16);
	__gp = . + 0x200000;
	.sdata : {
		_sdata = . ;
		*(.got.plt)
		*(.got)
		*(.srodata)
		*(.sdata)
		_esdata = . ;
	}
	.data : {
		_data = . ;
		*(.data)
		*(.data.*)
		*(.trailer)
		_edata = . ;
	}
	/DISCARD/ : {
		*(.comment)
		*(.note)
	}
}