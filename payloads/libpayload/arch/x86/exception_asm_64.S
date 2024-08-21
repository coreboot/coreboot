/*
 *
 * Copyright 2024 Google Inc.
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

	.align 16
	.global exception_stack_end
exception_stack_end:
	.quad 0
	.global exception_state
exception_state:
	.quad 0

/* Some temporary variables which are used while saving exception state. */
vector:
	.quad 0
error_code:
	.quad 0
old_rax:
	.quad 0
old_rcx:
	.quad 0

	.align 16

/*
 * Each exception vector has a small stub associated with it which sets aside
 * the error code, if any, records which vector we entered from, and calls
 * the common exception entry point. Some exceptions have error codes and some
 * don't, so we have a macro for each type.
 */

	.macro stub num
exception_stub_\num:
	movq $0, error_code
	movq $\num, vector
	jmp exception_common
	.endm

	.macro stub_err num
exception_stub_\num:
	pop error_code
	movq $\num, vector
	jmp exception_common
	.endm

	.altmacro
	.macro user_defined_stubs from, to
	stub \from
	.if \to-\from
	user_defined_stubs %(\from+1),\to
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
	 * At this point, on x86-64, on the stack there is:
	 * 0(%rsp) rip
	 * 8(%rsp) cs
	 * 16(%rsp) rflags
	 * 24(%rsp) rsp
	 * 32(%rsp) ss
	 *
	 * This section sets up the exception stack.
	 * It saves the old stack pointer (rsp) to preserve RIP, CS, RFLAGS and SS.
	 * Then sets up the new stack pointer to point to the exception stack area.
	 */
	movq %rax, old_rax
	movq %rcx, old_rcx

	mov %rsp, %rax
	movq exception_stack_end, %rsp
	/*
	 * The `exception_state` struct is not 16-byte aligned.
	 * Push an extra 8 bytes to ensure the stack pointer
	 * is 16-byte aligned before calling exception_dispatch.
	 */
	push $0

	/*
	 * Push values onto the top of the exception stack to form an
	 * exception state structure.
	 */
	push vector
	push error_code

	/* push of the gs, fs, es, ds, ss and cs */
	mov %gs, %rcx
	movl %ecx, -4(%rsp)   /* gs */
	mov %fs, %rcx
	movl %ecx, -8(%rsp)   /* fs */
	movl $0, -12(%rsp)    /* es */
	movl $0, -16(%rsp)    /* ds */
	movq 32(%rax), %rcx
	movl %ecx, -20(%rsp)  /* ss */
	movq 8(%rax), %rcx
	movl %ecx, -24(%rsp)  /* cs */
	sub $24, %rsp

	push 16(%rax)         /* rflags */
	push (%rax)           /* rip */
	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %r9
	push %r8
	push 24(%rax)         /* rsp */
	push %rbp
	push %rdi
	push %rsi
	push %rdx
	push old_rcx          /* rcx */
	push %rbx
	push old_rax          /* rax */

	/*
	 * Call the C exception handler. It will find the exception state
	 * using the exception_state global pointer. Not
	 * passing parameters means we don't have to worry about what ABI
	 * is being used.
	 */
	mov %rsp, exception_state
	call exception_dispatch

	/*
	 * Restore state from the exception state structure, including any
	 * changes that might have been made.
	 */
	pop old_rax
	pop %rbx
	pop old_rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rbp
	lea exception_stack, %rax
	pop 24(%rax)          /* rsp */
	pop %r8
	pop %r9
	pop %r10
	pop %r11
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	pop (%rax)            /* rip */
	pop 16(%rax)          /* rflags */

	/* pop of the gs, fs, es, ds, ss and cs */
	movl (%rsp), %ecx
	movq %rcx, 8(%rax)    /* cs */
	movl 4(%rsp), %ecx
	movq %rcx, 32(%rax)   /* ss */
	movl 16(%rsp), %ecx
	mov %rcx, %fs         /* fs */
	movl 20(%rsp), %ecx
	mov %rcx, %gs         /* gs */

	mov %rax, %rsp
	movq old_rax, %rax
	movq old_rcx, %rcx

	iretq

/*
 * We need segment selectors for the IDT, so we need to know where things are
 * in the GDT. We set one up here which is pretty standard and largely copied
 * from coreboot.
 */
	.align 16
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
	.byte 0x00, 0x92, 0xcf, 0x00

	/* selgdt 0x20, flat x64 code segment */
	.word 0xffff, 0x0000
	.byte 0x00, 0x9b, 0xaf, 0x00
gdt_end:

/* GDT pointer for use with lgdt */
.global gdt_ptr
gdt_ptr:
	.word gdt_end - gdt - 1
	.quad gdt

	/*
	 * Record the target and construct the actual entry at init time. This
	 * is necessary because the linker doesn't want to construct the entry
	 * for us.
	 */
	.macro interrupt_gate target
	.word 0       /* patchable */
	.word 0x20    /* Target code segment selector */
	.word 0xee00  /* Present, Type 64-bit Interrupt Gate */
	.word 0       /* patchable */
	.quad \target /* patchable */
	.endm

	.altmacro
	.macro	user_defined_gates from, to
	interrupt_gate	exception_stub_\from
	.if	\to-\from
	user_defined_gates	%(\from+1),\to
	.endif
	.endm

	.align 16
	.global idt
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
	.quad idt

.section .text.exception_init_asm
.globl exception_init_asm
.type exception_init_asm, @function

exception_init_asm:
	/* Set up IDT entries */
	mov $idt, %rax
1:
	movq 8(%rax), %rdi
	movw %di, (%rax)   /* procedure entry point offset bits 0..15 */
	shr $16, %rdi
	movw %di, 6(%rax)  /* procedure entry point offset bits 16..31 */
	shr $16, %rdi
	movl %edi, 8(%rax) /* procedure entry point offset bits 32..63 */
	movl $0, 12(%rax)  /* reserved */
	add $16, %rax
	cmp $idt_end, %rax
	jne 1b

	/* Load the IDT */
	lidt idt_ptr
	ret
