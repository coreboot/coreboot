/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/asm.h>

/*
 * Note: This board uses boot flow: TFA -> coreboot.
 */
ENTRY(_start)

	/* TF-A arg which contains a pointer to fdt */
	ldr	x1, =_fdt_pointer
	str	x0, [x1]

	msr SPSel, #0 /* use SP_EL0 */

	/* ==== stack init from arm64_init_cpu ==== */
	ldr	x2, =0xdeadbeefdeadbeef
	ldr	x0, =_stack
	ldr	x1, =_estack
1:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	bne	1b

	sub	sp, x0, #16

	/* ==== END ==== */

	/* Jump to main() in DRAM. */
	bl	main
ENDPROC(_start)
