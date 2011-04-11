/*
 * Copyright 2002 Eric Biederman
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 */
#include <cpu/x86/post_code.h>

	/* clear boot_complete flag */
	xorl	%ebp, %ebp
__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* clear direction flag */

	movl	%ebp, %esi

	movl $ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	pushl %esi
	call copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

