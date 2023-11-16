/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Early initialization code for sbsa-ref machine
 */

#include <arch/asm.h>

ENTRY(_start)

	/* TF-A arg which contains a pointer to fdt */
	ldr	x1, =_fdt_pointer
	str	x0, [x1]

	/* Setup CPU. */
	/* bl      arm64_init_cpu */

	/* ==== stack init from arm64_init_cpu ==== */

	msr SPSel, #0 /* use SP_EL0 */

	ldr	x2, =0xdeadbeefdeadbeef
	ldr	x0, =_stack
	ldr	x1, =_estack
1:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	bne	1b

	sub	sp, x0, #16

	/* ==== END ==== */

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
