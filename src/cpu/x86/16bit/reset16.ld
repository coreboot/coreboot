/* SPDX-License-Identifier: GPL-2.0-only */

/*
 *      _ROMTOP                 : The top of the ROM used where we
 *				  need to put the reset vector.
 */

SECTIONS {
	/* Trigger an error if I have an unuseable start address */
	_bogus = ASSERT(_start16bit >= 0xffff0000, "_start16bit too low. Please report.");
	_ROMTOP = 0xfffffff0;
	. = _ROMTOP;
	.reset . : {
		*(.reset);
		. = 15;
		BYTE(0x00);
	}
}
