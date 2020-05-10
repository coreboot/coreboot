/* SPDX-License-Identifier: GPL-2.0-only */

SECTIONS {
	. = (0xffffffff - CONFIG_ID_SECTION_OFFSET) - (__id_end - __id_start) + 1;
	.id (.): {
		KEEP(*(.id))
	}
}
