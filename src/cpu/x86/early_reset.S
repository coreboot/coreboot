/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * input %esp: return address (not pointer to return address!)
 * clobber the content of eax, ecx, edx
 */

#include <cpu/x86/mtrr.h>

.code32
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
