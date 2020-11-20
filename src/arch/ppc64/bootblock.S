/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early initialization code for POWER8.
 */

.section ".text._start", "ax", %progbits
.globl _start
.org 0x100, 0xff
_start:
	b _start
