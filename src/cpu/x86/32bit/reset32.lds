/*
 *      _ROMTOP                 : The top of the rom used where we
 *				  need to put the reset vector.
 */

SECTIONS {
	_ROMTOP = _ROMBASE + ROM_IMAGE_SIZE - 0x10;
	. = _ROMTOP;
	.reset (.): {
		*(.reset)
		. = 15 ;
		BYTE(0x00);
	} 
}
