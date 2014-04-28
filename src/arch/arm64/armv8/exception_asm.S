/*
 * This file is part of the libpayload project.
 *
 * Copyright 2014 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/asm.h>

	.text

	.align 11
	.global exception_table
exception_table:
	.align 7
	bl	exception_prologue
	bl	exception_sync_el0
	.align 7
	bl	exception_prologue
	bl	exception_irq_el0
	.align 7
	bl	exception_prologue
	bl	exception_fiq_el0
	.align 7
	bl	exception_prologue
	bl	exception_serror_el0
	.align 7
	bl	exception_prologue
	bl	exception_sync
	.align 7
	bl	exception_prologue
	bl	exception_irq
	.align 7
	bl	exception_prologue
	bl	exception_fiq
	.align 7
	bl	exception_prologue
	bl	exception_serror

/*
 * Save state (register file + ELR) to stack
 * and set arguments x0 and x1 for exception call
 */
ENTRY(exception_prologue)
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	/* FIXME: Don't assume always running in EL2 */
	mrs	x1, elr_el2
	stp	x1, x0, [sp, #-16]!

	mrs	x1, esr_el2
	mov	x0, sp

	ret
ENDPROC(exception_prologue)

	.global exception_stack_end
exception_stack_end:
	.quad 0

exception_handler:
	.word 0

	.global set_vbar
set_vbar:
	msr	vbar_el2, x0
	ret
