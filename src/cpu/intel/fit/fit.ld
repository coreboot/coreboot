/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

SECTIONS {
	. = 0xffffffc0;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer))
	}
}
