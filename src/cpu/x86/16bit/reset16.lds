/*
 *      _ROMTOP                 : The top of the rom used where we
 *				  need to put the reset vector.
 */

SECTIONS {
	_ROMTOP = (_ROMBASE >= 0xffff0000)? 0xfffffff0 : 0xffff0;
	. = _ROMTOP;
	.reset . : {
		*(.reset)
		. = 15 ;
		BYTE(0x00);
	} 
}
