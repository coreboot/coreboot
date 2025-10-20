/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/asm.h>

/*
 * Our entry point
 */
ENTRY(_entry)

	/* Setup new stack */
	lla t0, 1f
	ld sp, 0(t0)
	//mov sp, x1

	/* Let's rock. */
	j start_main

	ret
ENDPROC(_entry)

.section .entry, "ax", %progbits
.align 4
1:
.quad	_stack
