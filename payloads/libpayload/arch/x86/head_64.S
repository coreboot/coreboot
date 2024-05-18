/*
 *
 * Copyright (C) 2024 Google Inc.
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

#define IA32_EFER	0xC0000080
#define  EFER_LME	(1 << 8)

	.code32
	.global _entry
	.section .text._entry
	.align 4

/*
 * WARNING: Critical Code Section - 32/64-bit Compatibility
 * This code between `_entry` and `jnz _init64` is executed during system initialization.
 * It MUST function correctly regardless of whether the system is booting in:
 *   - 32-bit protected mode
 *   - 64-bit long mode
 * To achieve this, ONLY use instructions that produce identical binary output in both modes.
 * Thoroughly test ALL modifications to this section in BOTH 32-bit and 64-bit boot
 * environments.
 */

_entry:

	/* Add multiboot header and jump around it when building with multiboot support. */
#if CONFIG(LP_MULTIBOOT)
	#include "multiboot_header.inc"
#endif
	/* No interrupts, please. */
	cli

	movl $IA32_EFER, %ecx
	rdmsr
	testl $EFER_LME, %eax
	jnz _init64

	lgdt %cs:gdt_ptr

	/* save pointer to coreboot tables */
	movl 4(%esp), %eax
	/*
	 * NOTE: coreboot tables has passed over the top of the stack
	 * while calling in protected mode.
	 */
	movl %eax, cb_header_ptr

	call init_page_table
	movl $pm4le, %eax

	/* load identity mapped page tables */
	movl %eax, %cr3

	/* enable PAE */
	movl %cr4, %eax
	btsl $5, %eax
	movl %eax, %cr4

	/* enable long mode */
	movl $(IA32_EFER), %ecx
	rdmsr
	btsl $8, %eax
	wrmsr

	/* enable paging */
	movl %cr0, %eax
	btsl $31, %eax
	movl %eax, %cr0

	/* Jump to selgdt 0x20, flat x64 code segment */
	ljmp $0x20, $_entry64

.code64
.align 16
_init64:
	movabs	$gdt_ptr, %rax
	lgdt	(%rax)

	/*
	 * Note: The `cb_header_ptr` has passed as the first argument
	 * to the x86-64 calling convention.
	 */
	movq %rdi, cb_header_ptr

	call init_page_table
	movq $pm4le, %rax

	/* load identity mapped page tables */
	movq %rax, %cr3

_entry64:
	/* Store current stack pointer and set up new stack. */
	movq %rsp, %rax
	movabs	$_estack, %rsp

	push %rax

	fninit
	movq %cr0, %rax
	andq $0xFFFFFFFFFFFFFFFB, %rax	/* clear EM */
	orq $0x00000022, %rax	/* set MP, NE */
	movq %rax, %cr0

	movq %cr4, %rax
	orq $0x00000600, %rax	/* set OSFXSR, OSXMMEXCPT */
	movq %rax, %cr4

	/* Let's rock. */
	call start_main

	/* %rax has the return value - pass it on unmolested */
_leave:
	/* Restore old stack. */
	pop %rsp

	/* Return to the original context. */
	ret
