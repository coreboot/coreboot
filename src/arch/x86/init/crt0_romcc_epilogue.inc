/*
 * Copyright 2002 Eric Biederman
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 */
#include <cpu/x86/post_code.h>

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* clear direction flag */

	movl $ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	call copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

