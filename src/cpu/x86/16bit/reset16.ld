/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
