
ENTRY(_start)

SECTIONS
{
	. = 0x1000;
	.text . : {
		. = ALIGN(16);
		_start = . ;
		*(.rom.text);
		*(.text)
		. = ALIGN(16);
	}
	.data . : {
		. = ALIGN(16);
		*(.rom.data);
		*(.data)
		. = ALIGN(16);
	}
}