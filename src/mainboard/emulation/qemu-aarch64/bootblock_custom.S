/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <arch/asm.h>

ENTRY(_start)
	/* Setup CPU. */
	bl      arm64_init_cpu

	/* Get code positions. */
	ldr	x1, =_flash
	ldr	x0, =_bootblock

	/* Calculate bootblock size. */
	ldr     x2, =_ebootblock
	sub     x2, x2, x0

	/* Call memcpy in arch/arm64/memcpy.S */
	bl	memcpy
	dmb     sy

	/* Calculate relocation offset between bootblock in flash and in DRAM. */
	ldr	x0, =_flash
	ldr	x1, =_bootblock
	sub	x1, x1, x0

	/* Jump to main() in DRAM. */
	adr	x0, main
	add	x0, x0, x1
	blr	x0
ENDPROC(_start)
