/* SPDX-License-Identifier: GPL-2.0-only */

	.section ".reset", "ax", %progbits
	.code16
.globl	_start
_start:
	.byte  0xe9
	.int   _start16bit - ( . + 2 )
	/* Note: The above jump is hand coded to work around bugs in binutils.
	 * 5 byte are used for a 3 byte instruction.  This works because x86
	 * is little endian and allows us to use supported 32bit relocations
	 * instead of the weird 16 bit relocations that binutils does not
	 * handle consistently between versions because they are used so rarely.
	 */
