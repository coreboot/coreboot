/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Early initialization code for sbsa-ref machine
 */

#include <arch/asm.h>

ENTRY(_start)
	msr SPSel, #0 /* use SP_EL0 */

	/* Initialize stack with sentinel value to later check overflow. */
	ldr	x9, =0xdeadbeefdeadbeef
	ldr	x10, =_stack
	ldr	x11, =_estack
1:
	stp	x9, x9, [x10], #16
	cmp	x10, x11
	bne	1b

	ldr x1, =_estack
	mov x2, #8192
	sub sp, x1, #16
	sub x0, x1, x2
	//bl exception_init_asm
	bl main
	//bl bootblock_main_with_timestamp
ENDPROC(_start)
