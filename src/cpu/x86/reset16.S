/* SPDX-License-Identifier: GPL-2.0-only */

	.section ".reset", "ax", %progbits
	.code16
.globl	_start
_start:
	jmp   _start16bit
