/* See make-spike-elf.sh */

ENTRY(_start);
SECTIONS
{
	. = 0x80000000;
	_start = .;
	.data : {
		*(.data)
	}

	tohost = .;
	. = . + 8;
	fromhost = .;
}
