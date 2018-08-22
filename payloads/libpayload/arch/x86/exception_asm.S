/*
 * This file is part of the libpayload project.
 *
 * Copyright 2013 Google Inc.
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

	.align 4
	.global exception_stack_end
exception_stack_end:
	.long 0
	.global exception_state
exception_state:
	.long 0

/* Some temporary variables which are used while saving exception state. */
vector:
	.long 0
error_code:
	.long 0
old_esp:
	.long 0
old_eax:
	.long 0

	.align 8

/*
 * Each exception vector has a small stub associated with it which sets aside
 * the error code, if any, records which vector we entered from, and calls
 * the common exception entry point. Some exceptions have error codes and some
 * don't, so we have a macro for each type.
 */

	.macro stub num
exception_stub_\num:
	movl	$0, error_code
	movl	$\num, vector
	jmp	exception_common
	.endm

	.macro stub_err num
exception_stub_\num:
	popl	error_code
	movl	$\num, vector
	jmp	exception_common
	.endm

	.altmacro
	.macro	user_defined_stubs from, to
	stub	\from
	.if	\to-\from
	user_defined_stubs	%(from+1),\to
	.endif
	.endm

	stub 0
	stub 1
	stub 2
	stub 3
	stub 4
	stub 5
	stub 6
	stub 7
	stub_err 8
	stub 9
	stub_err 10
	stub_err 11
	stub_err 12
	stub_err 13
	stub_err 14
	stub 15
	stub 16
	stub_err 17
	stub 18
	stub 19
	stub 20
	stub 21
	stub 22
	stub 23
	stub 24
	stub 25
	stub 26
	stub 27
	stub 28
	stub 29
	stub_err 30
	stub 31
	/* Split the macro so we avoid a stack overflow. */
	user_defined_stubs 32, 63
	user_defined_stubs 64, 127
	user_defined_stubs 128, 191
	user_defined_stubs 192, 255

exception_common:
	/*
	 * Save off the stack pointer and old eax value and install the
	 * exception stack. eax points to the old stack which has the
	 * exception ip, cs, and flags.
	 */
	mov	%esp, old_esp
	addl	$12, old_esp
	mov	%eax, old_eax
	mov	%esp, %eax
	mov	exception_stack_end, %esp

	/*
	 * Push values onto the top of the exception stack to form an
	 * exception state structure.
	 */
	pushl	vector
	pushl	error_code
	pushl	%gs
	pushl	%fs
	pushl	%es
	pushl	%ds
	pushl	%ss
	pushl	4(%eax)
	pushl	8(%eax)
	pushl	(%eax)
	pushl	%edi
	pushl	%esi
	pushl	%ebp
	pushl	old_esp
	pushl	%ebx
	pushl	%edx
	pushl	%ecx
	pushl	old_eax

	/*
	 * Call the C exception handler. It will find the exception state
	 * using the exception_state global pointer. Not
	 * passing parameters means we don't have to worry about what ABI
	 * is being used.
	 */
	mov	%esp, exception_state
	call	exception_dispatch

	/*
	 * Restore state from the exception state structure, including any
	 * changes that might have been made.
	 */
	popl	old_eax
	popl	%ecx
	popl	%edx
	popl	%ebx
	popl	old_esp

	mov	old_esp, %eax
	subl	$12, %eax

	popl	%ebp
	popl	%esi
	popl	%edi
	popl	(%eax)
	popl	8(%eax)
	popl	4(%eax)
	popl	%ss
	popl	%ds
	popl	%es
	popl	%fs
	popl	%gs

	mov	%eax, %esp
	mov	old_eax, %eax

	/* Return from the exception. */
	iretl

/*
 * We need segment selectors for the IDT, so we need to know where things are
 * in the GDT. We set one up here which is pretty standard and largely copied
 * from coreboot.
 */
	.align 8
gdt:
	/* selgdt 0, unused */
	.word 0x0000, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00

	/* selgdt 8, unused */
	.word 0x0000, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00

	/* selgdt 0x10, flat 4GB code segment */
        .word 0xffff, 0x0000
	.byte 0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x18, flat 4GB data segment */
	.word 0xffff, 0x0000
	.byte 0x00, 0x93, 0xcf, 0x00
gdt_end:

/* GDT pointer for use with lgdt */
gdt_ptr:
	.word	gdt_end - gdt - 1
	.long	gdt

	/*
	 * Record the target and construct the actual entry at init time. This
	 * is necessary because the linker doesn't want to construct the entry
	 * for us.
	 */
	.macro interrupt_gate target
	.long \target
	.long \target
	.endm

	.altmacro
	.macro	user_defined_gates from, to
	interrupt_gate	exception_stub_\from
	.if	\to-\from
	user_defined_gates	%(from+1),\to
	.endif
	.endm

	.align 8
	.global	idt
idt:
	interrupt_gate exception_stub_0
	interrupt_gate exception_stub_1
	interrupt_gate exception_stub_2
	interrupt_gate exception_stub_3
	interrupt_gate exception_stub_4
	interrupt_gate exception_stub_5
	interrupt_gate exception_stub_6
	interrupt_gate exception_stub_7
	interrupt_gate exception_stub_8
	interrupt_gate exception_stub_9
	interrupt_gate exception_stub_10
	interrupt_gate exception_stub_11
	interrupt_gate exception_stub_12
	interrupt_gate exception_stub_13
	interrupt_gate exception_stub_14
	interrupt_gate exception_stub_15
	interrupt_gate exception_stub_16
	interrupt_gate exception_stub_17
	interrupt_gate exception_stub_18
	interrupt_gate exception_stub_19
	interrupt_gate exception_stub_20
	interrupt_gate exception_stub_21
	interrupt_gate exception_stub_22
	interrupt_gate exception_stub_23
	interrupt_gate exception_stub_24
	interrupt_gate exception_stub_25
	interrupt_gate exception_stub_26
	interrupt_gate exception_stub_27
	interrupt_gate exception_stub_28
	interrupt_gate exception_stub_29
	interrupt_gate exception_stub_30
	interrupt_gate exception_stub_31
	user_defined_gates 32, 63
	user_defined_gates 64, 127
	user_defined_gates 128, 191
	user_defined_gates 192, 255
idt_end:

/* IDT pointer for use with lidt */
idt_ptr:
	.word idt_end - idt - 1
	.long idt

	.global exception_init_asm
exception_init_asm:
	/* Save eax so we can use it as a temporary variable. */
	pushl	%eax

	/* Install the GDT. */
	lgdt	gdt_ptr
	/* Load the segment registers from it. */
	ljmp	$0x10, $1f
1:	movl	$0x18, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs
	movl	%eax, %gs

	/*
	 * Loop over the entries which start out as two copies of the target
	 * address. We can turn them into real interrupt gates by selectively
	 * replacing certain bit fields.
	 */
	movl	$idt, %eax
1:
	andl	$0x0000ffff, (%eax)
	orl	$0x00100000, (%eax)
	andl	$0xffff0000, 4(%eax)
	orl	$0x0000ee00, 4(%eax)
	addl	$8, %eax
	cmp	$idt_end, %eax
	jne	1b

	/* Install the IDT. */
	lidt	idt_ptr

	/* Restore eax and return to the caller. */
	popl	%eax
	ret
