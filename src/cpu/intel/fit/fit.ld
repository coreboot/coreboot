/* SPDX-License-Identifier: GPL-2.0-only */

SECTIONS {
	. = 0xffffffc0;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer))
	}
}
