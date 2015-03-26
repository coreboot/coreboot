/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/*
 * This code is compiled for both arm64 and arm4, however the code is only
 * executed by the armv8 cores coming out of reset.
 */

#if !defined(__PRE_RAM__)
#define INST .inst
#else
#define INST .word
#endif

/*
 * The Denver cores come up in aarch32 mode. In order to transition to
 * 64-bit mode a write to the RMR (reest mangement register) with the
 * AA64 bit (0) set while setting RR (reset request bit 1).
 */
.align 6
.global reset_entry_32bit
reset_entry_32bit:
	INST	0xe3a00003	/* mov r0, #3 */
	INST	0xee0c0f50	/* mcr 15, 0, r0, cr12, cr0, {2} */
	INST	0xeafffffe	/* b . */
