/*
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
#include <arch/exception.h>

	/* Macro for exception entry
	 * Store x30 before any branch
	 * Branch to exception_prologue to save rest and switch stacks
	 * Move exception id into x1
	 * Branch to exception_dispatch (exception C entry point)
	 * Branch to exception_return to return from exception
	 */
.macro eentry lbl id
	.align 7
\lbl:
	/* Note: SP points to exception_state (see exception_set_state_ptr) */
	str	x30, [sp, #EXCEPTION_STATE_REG(30)]
	bl	exception_prologue
	mov	x1, \id
	bl	exception_dispatch
	b	exception_return
.endm

	/* Exception table has 16 entries and each of 128 bytes
	 * Hence, 16 * 128 = 2048. Thus, 11 passed as parameter
	 * to align
	 */

	.align  11
	.global exception_table
exception_table:

eentry	sync_sp0,#0
eentry	irq_sp0,#1
eentry	fiq_sp0,#2
eentry	serror_sp0,#3
eentry	sync_spx,#4
eentry	irq_spx,#5
eentry	fiq_spx,#6
eentry	serror_spx,#7
eentry	sync_elx_64,#8
eentry	irq_elx_64,#9
eentry	fiq_elx_64,#10
eentry	serror_elx_64,#11
eentry	sync_elx_32,#12
eentry	irq_elx_32,#13
eentry	fiq_elx_32,#14
eentry	serror_elx_32,#15

/* This code must match the layout of struct exception_state (minus x30) */
ENTRY(exception_prologue)
	/* Save registers x0-x29 */
	stp	x28, x29, [sp, #EXCEPTION_STATE_REG(28)]
	stp	x26, x27, [sp, #EXCEPTION_STATE_REG(26)]
	stp	x24, x25, [sp, #EXCEPTION_STATE_REG(24)]
	stp	x22, x23, [sp, #EXCEPTION_STATE_REG(22)]
	stp	x20, x21, [sp, #EXCEPTION_STATE_REG(20)]
	stp	x18, x19, [sp, #EXCEPTION_STATE_REG(18)]
	stp	x16, x17, [sp, #EXCEPTION_STATE_REG(16)]
	stp	x14, x15, [sp, #EXCEPTION_STATE_REG(14)]
	stp	x12, x13, [sp, #EXCEPTION_STATE_REG(12)]
	stp	x10, x11, [sp, #EXCEPTION_STATE_REG(10)]
	stp	x8, x9, [sp, #EXCEPTION_STATE_REG(8)]
	stp	x6, x7, [sp, #EXCEPTION_STATE_REG(6)]
	stp	x4, x5, [sp, #EXCEPTION_STATE_REG(4)]
	stp	x2, x3, [sp, #EXCEPTION_STATE_REG(2)]
	stp	x0, x1, [sp, #EXCEPTION_STATE_REG(0)]

	/* Save the stack pointer and SPSR */
	mrs	x1, sp_el0
	mrs	x0, spsr_el2
	stp	x0, x1, [sp, #EXCEPTION_STATE_SPSR]

	/* Save return address (ELR) and exception syndrome */
	mrs	x1, esr_el2
	mrs	x0, elr_el2
	stp	x0, x1, [sp, #EXCEPTION_STATE_ELR]

	/* Now switch to the actual exception stack. Keep a pointer to the
	   exception_state structure in x0 as an argument for dispatch(). */
	mov	x0, sp
	adrp	x1, exception_stack_end
	ldr	x1, [x1, :lo12:exception_stack_end]
	msr	SPSel, #0
	mov	sp, x1

	ret
ENDPROC(exception_prologue)

ENTRY(exception_return)
	/* Switch SP back to the exception_state structure */
	msr	SPSel, #1

	/* Restore return address (ELR) -- skip ESR (unneeded for return) */
	ldr	x0, [sp, #EXCEPTION_STATE_ELR]
	msr	elr_el2, x0

	/* Restore stack pointer and SPSR */
	ldp	x0, x1, [sp, #EXCEPTION_STATE_SPSR]
	msr	spsr_el2, x0
	msr	sp_el0, x1

	/* Restore all registers (x0-x30) */
	ldp	x0, x1, [sp, #EXCEPTION_STATE_REG(0)]
	ldp	x2, x3, [sp, #EXCEPTION_STATE_REG(2)]
	ldp	x4, x5, [sp, #EXCEPTION_STATE_REG(4)]
	ldp	x6, x7, [sp, #EXCEPTION_STATE_REG(6)]
	ldp	x8, x9, [sp, #EXCEPTION_STATE_REG(8)]
	ldp	x10, x11, [sp, #EXCEPTION_STATE_REG(10)]
	ldp	x12, x13, [sp, #EXCEPTION_STATE_REG(12)]
	ldp	x14, x15, [sp, #EXCEPTION_STATE_REG(14)]
	ldp	x16, x17, [sp, #EXCEPTION_STATE_REG(16)]
	ldp	x18, x19, [sp, #EXCEPTION_STATE_REG(18)]
	ldp	x20, x21, [sp, #EXCEPTION_STATE_REG(20)]
	ldp	x22, x23, [sp, #EXCEPTION_STATE_REG(22)]
	ldp	x24, x25, [sp, #EXCEPTION_STATE_REG(24)]
	ldp	x26, x27, [sp, #EXCEPTION_STATE_REG(26)]
	ldp	x28, x29, [sp, #EXCEPTION_STATE_REG(28)]
	ldr	x30, [sp, #EXCEPTION_STATE_REG(30)]

	/* Return from exception */
	eret
ENDPROC(exception_return)

	/*
	 * We have two stack pointers on AArch64: SP_EL0 (which despite the
	 * naming is used in all ELs) and SP_EL2. We can select which one to
	 * use by writing to SPSel. Normally we're using SP_EL0, but on
	 * exception entry it automatically switches to SP_EL2.
	 *
	 * It is important for exception reentrancy to switch back to SP_EL0
	 * while handling the exception. We only need SP_EL2 for the assembly
	 * exception entry and exit code that stores all register state
	 * (including the old SP_EL0, before we switch to the real exception
	 * stack). Rather than having yet another stack to push/pop those
	 * register values on so that we can later sort them into the
	 * exception_state structure, it's much easier to just make SP_EL2 point
	 * directly to exception_state and just use it as a normal base register
	 * rather than a real stack. This function sets that up.
	 */
ENTRY(exception_set_state_ptr)
	msr	SPSel, #1
	mov	sp, x0
	msr	SPSel, #0
	ret
ENDPROC(exception_set_state_ptr)
