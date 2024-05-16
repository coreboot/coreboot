/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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

	.code32
	.global _entry
	.section .text._entry
	.align 4

/*
 * Our entry point - assume that the CPU is in 32 bit protected mode and
 * all segments are in a flat model. That's our operating mode, so we won't
 * change anything.
 */
_entry:

	/* Add multiboot header and jump around it when building with multiboot support. */
#if CONFIG(LP_MULTIBOOT)
	#include "multiboot_header.inc"
#endif
	/* No interrupts, please. */
	cli

	/* save pointer to coreboot tables */
	movl 4(%esp), %eax
	movl %eax, cb_header_ptr

	/* Store current stack pointer and set up new stack. */
	movl %esp, %eax
	movl $_estack, %esp
	pushl %eax

	/* Enable special x86 functions if present. */
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx

	movl $0, %eax
	cpuid
	/* Test if CPUID(eax=1) is available. */
	test %eax, %eax
	je cpuid_done

	/* Get CPU features. */
	movl $1, %eax
	cpuid

cpuid_fpu:
	/* Test if x87 FPU is present */
	test $1, %edx
	je cpuid_sse

	fninit
	movl %cr0, %eax
	andl $0xFFFFFFFB, %eax	/* clear EM */
	orl $0x00000022, %eax	/* set MP, NE */
	movl %eax, %cr0

cpuid_sse:
	/* Test if SSE is available */
	test $0x02000000, %edx
	je cpuid_done

	movl %cr4, %eax
	orl $0x00000600, %eax	/* set OSFXSR, OSXMMEXCPT */
	movl %eax, %cr4

cpuid_done:
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax

	/* Let's rock. */
	call start_main

	/* %eax has the return value - pass it on unmolested */
_leave:
	/* Restore old stack. */
	popl %esp

	/* Return to the original context. */
	ret
