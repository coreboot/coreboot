/*
 *      _ROMTOP                 : The top of the rom used where we
 *				  need to put the reset vector.
 */
	_ROMTOP = (_ROMBASE >= 0xffff0000)? 0xfffffff0 : 0xffff0;

SECTIONS {
	. = _ROMTOP;
	.text.reset_vector (.): {
		*(.text.reset_vector)
		. = 15 ;
		BYTE(0x00);
	} 
	/DISCARD/ : {
		*(*)
	}
}
