/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * input %esp: return address (not pointer to return address!)
 * clobber the content of eax, ecx, edx
 */

#include <cpu/x86/mtrr.h>

.section .text
.global check_mtrr

check_mtrr:
	/* Use the MTRR default type MSR as a proxy for detecting INIT#.
	 * Reset the system if any known bits are set in that MSR. That is
	 * an indication of the CPU not being properly reset. */

check_for_clean_reset:
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN), %eax
	cmp	$0, %eax
	jnz	warm_reset
	jmp	*%esp
	/* perform warm reset */
warm_reset:
	movw	$0xcf9, %dx
	movb	$0x06, %al
	outb	%al, %dx
	/* Should not reach this*/
.Lhlt:
	hlt
	jmp	.Lhlt
