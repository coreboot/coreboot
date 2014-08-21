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

.macro eentry lbl id
	.align 7
\lbl:
	stp	x30, xzr, [sp, #-16]!
	bl	exception_prologue
	mov	x1, \id
	bl	exception_handler
.endm

	.align	11
	.global exception_table

exception_table:

eentry	sync_el3_sp0,#0
eentry	irq_el3_sp0,#1
eentry	fiq_el3_sp0,#2
eentry	serror_el3_sp0,#3
eentry	sync_el3_sp3,#4
eentry	irq_el3_sp3,#5
eentry	fiq_el3_sp3,#6
eentry	serror_el3_sp3,#7
eentry	sync_elx_64,#8
eentry	irq_elx_64,#9
eentry	fiq_elx_64,#10
eentry	serror_elx_64,#11
eentry	sync_elx_32,#12
eentry	irq_elx_32,#13
eentry	fiq_elx_32,#14
eentry	serror_elx_32,#15

exception_prologue:
	/* Save all registers x0-x29 */
	stp	x28, x29, [sp, #-16]!
	stp	x26, x27, [sp, #-16]!
	stp	x24, x25, [sp, #-16]!
	stp	x22, x23, [sp, #-16]!
	stp	x20, x21, [sp, #-16]!
	stp	x18, x19, [sp, #-16]!
	stp	x16, x17, [sp, #-16]!
	stp	x14, x15, [sp, #-16]!
	stp	x12, x13, [sp, #-16]!
	stp	x10, x11, [sp, #-16]!
	stp	x8, x9, [sp, #-16]!
	stp	x6, x7, [sp, #-16]!
	stp	x4, x5, [sp, #-16]!
	stp	x2, x3, [sp, #-16]!
	stp	x0, x1, [sp, #-16]!

	/* Save the exception reason on stack */
	mrs	x1,  esr_el3

	/* Save the return address on stack */
	mrs	x0, elr_el3
	stp	x0, x1, [sp, #-16]!

	ret

exception_handler:
	/* Save address of saved registers into x0
	 * This acts as first argument to exception_dispatch
	 */
	mov	x0, sp
	bl	exception_dispatch

	/* Pop return address and exception reason saved on stack */
	ldp	x0, x1, [sp], #16
	msr	elr_el3, x0
	msr	esr_el3, x1
	/* Pop registers x0-x30 */
	ldp	x0, x1, [sp], #16
	ldp	x2, x3, [sp], #16
	ldp	x4, x5, [sp], #16
	ldp	x6, x7, [sp], #16
	ldp	x8, x9, [sp], #16
	ldp	x10, x11, [sp], #16
	ldp	x12, x13, [sp], #16
	ldp	x14, x15, [sp], #16
	ldp	x16, x17, [sp], #16
	ldp	x18, x19, [sp], #16
	ldp	x20, x21, [sp], #16
	ldp	x22, x23, [sp], #16
	ldp	x24, x25, [sp], #16
	ldp	x26, x27, [sp], #16
	ldp	x28, x29, [sp], #16
	ldp	x30, xzr, [sp], #16
	eret

	.global set_vbar
set_vbar:
	/* Initialize the exception table address in vbar for EL3 */
	msr	vbar_el3, x0
	dsb	sy
	isb
	ret
