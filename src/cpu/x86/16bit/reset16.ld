/* SPDX-License-Identifier: GPL-2.0-only */

/* _RESET_VECTOR: typically the top of the ROM */

SECTIONS {
	/* Trigger an error if I have an unuseable start address */
	_TOO_LOW = CONFIG_X86_RESET_VECTOR - 0xfff0;
	_bogus = ASSERT(_start16bit >= _TOO_LOW, "_start16bit too low. Please report.");

	. = CONFIG_X86_RESET_VECTOR;
	.reset . : {
		*(.reset);
		. = 15;
		BYTE(0x00);
	}
}
