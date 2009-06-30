/*
 *      _ROMTOP                 : The top of the rom used where we
 *				  need to put the reset vector.
 */

SECTIONS {
	_ROMTOP = CONFIG_ROMBASE + CONFIG_ROM_IMAGE_SIZE - 0x10;
	. = _ROMTOP;
	.reset (.): {
		*(.reset)
		. = 15 ;
		BYTE(0x00);
	} 
}
