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

	/* For now: use CONFIG_RAMBASE + 1MB - 64K (counting downwards) as stack. This
	 * makes sure that we stay completely within the 1M-64K of memory that we
	 * preserve for suspend/resume.
	 */

#ifndef HIGH_MEMORY_SAVE
#warning Need a central place for HIGH_MEMORY_SAVE
#define HIGH_MEMORY_SAVE ( (1024 - 64) * 1024 )
#endif
	movl $(CONFIG_RAMBASE + HIGH_MEMORY_SAVE), %esp
	movl	%esp, %ebp
	pushl %esi
	call copy_and_run

.Lhlt:	
	post_code(0xee)
	hlt
	jmp	.Lhlt

