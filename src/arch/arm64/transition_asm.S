/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 */

/*
 * transition_asm.S: This file handles the entry and exit from an exception
 *
 * Flow: exception --> exc_vectors --> exc_entry --> exc_dispatch -->
 * exc_exit
 * Transition Flow: transition --> trans_switch --> exc_exit
 *
 * |---|		      Exception Entry				   |---|
 *
 * On exception entry, it saves all the xregs on SP_ELx since SP_ELx is
 * selected on entry. Some dummy pushes are performed to create space for
 * elx_state structure. It then passes pointer to this saved set of regs and
 * a unique id(for identifying exception) to exc_entry.
 *
 * |---|		      Exception Transition Dispatch		   |---|
 *
 * This is the C-component of exception entry. It does the work of initializing
 * the exc_state registers. Finally it calls exception dispatch implemented by
 * the user. This is point of no-return.
 *
 * |---|		      Exception Dispatch			   |---|
 *
 * User of this functionality is expected to implement exc_dispatch which
 * acts as entry point for it. Once exception handling is complete, the user
 * needs to call exc_exit with pointer to struct regs.
 *
 * |---|		      Exception Exit				   |---|
 *
 * Once exc_dispatch is done with handling the exception based on the id passed
 * to it, it needs to call exc_exit with pointer to struct regs. This is done to
 * unwind the exception stack by popping off all the xregs.
 *
 * |---|		      Exception Transition Exit			   |---|
 *
 * This routine makes SP_EL0 point to the regs structure passed and continues
 * onto the exception exit routine described above. This is the reason that
 * transition library does not handle initialization of SP_EL0 for the program
 * to be executed.
 */

#include <arch/asm.h>
#include <arch/lib_helpers.h>
#include <arch/transition.h>

.macro eentry lbl id
	.align 7
\lbl:
	stp	x29, x30, [sp, #STACK_PUSH_BYTES]!
	bl	exc_prologue
	mov	x1, \id
	mov	x0, sp
	b	exc_entry
.endm

/*
 * exc_vectors: Entry point for an exception
 */
ENTRY_WITH_ALIGN(exc_vectors, 11)

eentry	sync_curr_sp0, #EXC_VID_CUR_SP_EL0_SYNC
eentry	irq_curr_sp0, #EXC_VID_CUR_SP_EL0_IRQ
eentry	fiq_curr_sp0, #EXC_VID_CUR_SP_EL0_FIRQ
eentry	serror_curr_sp0, #EXC_VID_CUR_SP_EL0_SERR
eentry	sync_curr_spx, #EXC_VID_CUR_SP_ELX_SYNC
eentry	irq_curr_spx, #EXC_VID_CUR_SP_ELX_IRQ
eentry	fiq_curr_spx, #EXC_VID_CUR_SP_ELX_FIQ
eentry	serror_curr_spx, #EXC_VID_CUR_SP_ELX_SERR
eentry	sync_lower_64, #EXC_VID_LOW64_SYNC
eentry	irq_lower_64, #EXC_VID_LOW64_IRQ
eentry	fiq_lower_64, #EXC_VID_LOW64_FIQ
eentry	serror_lower_64, #EXC_VID_LOW64_SERR
eentry	sync_lower_32, #EXC_VID_LOW32_SYNC
eentry	irq_lower_32, #EXC_VID_LOW32_IRQ
eentry	fiq_lower_32, #EXC_VID_LOW32_FIQ
eentry	serror_lower_32, #EXC_VID_LOW32_SERR

ENDPROC(exc_vectors)

ENTRY(exc_prologue)
	stp	x27, x28, [sp, #STACK_PUSH_BYTES]!
	stp	x25, x26, [sp, #STACK_PUSH_BYTES]!
	stp	x23, x24, [sp, #STACK_PUSH_BYTES]!
	stp	x21, x22, [sp, #STACK_PUSH_BYTES]!
	stp	x19, x20, [sp, #STACK_PUSH_BYTES]!
	stp	x17, x18, [sp, #STACK_PUSH_BYTES]!
	stp	x15, x16, [sp, #STACK_PUSH_BYTES]!
	stp	x13, x14, [sp, #STACK_PUSH_BYTES]!
	stp	x11, x12, [sp, #STACK_PUSH_BYTES]!
	stp	x9, x10, [sp, #STACK_PUSH_BYTES]!
	stp	x7, x8, [sp, #STACK_PUSH_BYTES]!
	stp	x5, x6, [sp, #STACK_PUSH_BYTES]!
	stp	x3, x4, [sp, #STACK_PUSH_BYTES]!
	stp	x1, x2, [sp, #STACK_PUSH_BYTES]!
	/* xzr pushed as place holder for sp */
	stp	xzr, x0, [sp, #STACK_PUSH_BYTES]!
	/*
	 * xzr pushed as place holder for:
	 * 1. sp_elx and elr
	 */
	stp	xzr, xzr, [sp, #STACK_PUSH_BYTES]!
	/* 2. spsr and sp_el0 */
	stp	xzr, xzr, [sp, #STACK_PUSH_BYTES]!
	ret
ENDPROC(exc_prologue)

/*
 * trans_switch: Set SPSel to use SP_EL0
 * x0 = regs structure
 */
ENTRY(trans_switch)
	msr	SPSel, #SPSR_USE_L
	b	exc_exit
ENDPROC(trans_switch)

/*
 * exc_exit: Return from exception by restoring saved state of xregs
 * x0 = regs structure
 */
ENTRY(exc_exit)
	/* Unwind the stack by making sp point to regs structure */
	mov	 sp, x0
	/* Load registers x0-x30 */
	ldp	 xzr, x0, [sp], #STACK_POP_BYTES
	ldp	 x1, x2, [sp], #STACK_POP_BYTES
	ldp	 x3, x4, [sp], #STACK_POP_BYTES
	ldp	 x5, x6, [sp], #STACK_POP_BYTES
	ldp	 x7, x8, [sp], #STACK_POP_BYTES
	ldp	 x9, x10, [sp], #STACK_POP_BYTES
	ldp	 x11, x12, [sp], #STACK_POP_BYTES
	ldp	 x13, x14, [sp], #STACK_POP_BYTES
	ldp	 x15, x16, [sp], #STACK_POP_BYTES
	ldp	 x17, x18, [sp], #STACK_POP_BYTES
	ldp	 x19, x20, [sp], #STACK_POP_BYTES
	ldp	 x21, x22, [sp], #STACK_POP_BYTES
	ldp	 x23, x24, [sp], #STACK_POP_BYTES
	ldp	 x25, x26, [sp], #STACK_POP_BYTES
	ldp	 x27, x28, [sp], #STACK_POP_BYTES
	ldp	 x29, x30, [sp], #STACK_POP_BYTES
	eret
ENDPROC(exc_exit)

/*
 * exception_init_asm: Initialize VBAR and point SP_EL3 to exception stack.
 * x0 = end of exception stack
 */
ENTRY(exception_init_asm)
	msr	SPSel, #SPSR_USE_H
	mov	sp, x0
	msr	SPSel, #SPSR_USE_L

	adr	x0, exc_vectors
	msr	vbar_el3, x0
	dsb	sy
	isb
	ret
ENDPROC(exception_init_asm)
