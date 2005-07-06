/* 
 * init sections to place code running with cache as ram.
 * 
 * 2004 by Stefan Reinauer <stepan@openbios.org>
 */

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
