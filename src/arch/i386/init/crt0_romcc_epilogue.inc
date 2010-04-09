/* 
 * Copyright 2002 Eric Biederman
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 */

	/* clear boot_complete flag */
	xorl	%ebp, %ebp
__main:
	post_code(0x11)
	cld			/* clear direction flag */
	
	movl	%ebp, %esi

	/* FIXME: look for a proper place for the stack */
	movl	$0x4000000, %esp
	movl	%esp, %ebp
	pushl %esi
	call copy_and_run

.Lhlt:	
	post_code(0xee)
	hlt
	jmp	.Lhlt

