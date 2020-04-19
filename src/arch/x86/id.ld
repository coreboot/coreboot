/* SPDX-License-Identifier: GPL-2.0-only */

SECTIONS {
	. = (CONFIG_X86_RESET_VECTOR - CONFIG_ID_SECTION_OFFSET) + 0x10 - (__id_end - __id_start);
	.id (.): {
		KEEP(*(.id))
	}
}
