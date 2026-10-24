/* SPDX-License-Identifier: GPL-2.0-only */

SECTIONS
{
	.bs_init : {
		test_bootstate_begin = .;
		KEEP(*(.bs_init))
		test_bootstate_end = .;
	}
}
INSERT AFTER .data;
