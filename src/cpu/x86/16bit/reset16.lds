/*
 *      _ROMTOP                 : The top of the rom used where we
 *				  need to put the reset vector.
 */

SECTIONS {
	/* Trigger an error if I have an unuseable start address */
	_ROMTOP = (_start >= 0xffff0000) ? 0xfffffff0 : 0xffffffff8;
	. = _ROMTOP;
	.reset . : {
		*(.reset)
		. = 15 ;
		BYTE(0x00);
	} 
}

