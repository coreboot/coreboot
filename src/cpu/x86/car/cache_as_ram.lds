SECTIONS {
	.init . : {
		_init = .;
		*(.init.text);
		*(.init.rodata);
		*(.init.rodata.*);
		. = ALIGN(16);
		_einit = .;
	}

}
