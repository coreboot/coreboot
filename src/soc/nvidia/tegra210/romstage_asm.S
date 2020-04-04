/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/asm.h>
#include "stack.S"

	.section ".text", "ax", %progbits

ENTRY(main)
	stack_init stack_top=_estack stack_bottom=_stack seed=0 func=romstage
ENDPROC(main)
