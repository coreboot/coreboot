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

/*
 * For reference see "AMD64 Architecture Programmer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3.4
 *
 * Page table attributes: WB, User+Supervisor, Present, Writeable, Accessed, Dirty
 */

.section .bss
#define _PRES (1ULL << 0)
#define _RW   (1ULL << 1)
#define _US   (1ULL << 2)
#define _A    (1ULL << 5)
#define _D    (1ULL << 6)
#define _PS   (1ULL << 7)

.section .bss.pm4le
.global pm4le
.align 4096
pm4le:
.skip 8

.section .bss.main_page_table
.global main_page_table
.align 4096
main_page_table:
.skip 8192

.section .bss.extra_page_table
.global extra_page_table
.align 4096
extra_page_table:
.skip 32

/*
 * WARNING: 32-bit/64-bit Mode Compatibility for Page Table Initialization
 * This `init_page_table` function is designed to work in both 32-bit protected
 * mode AND 64-bit long mode.
 *
 * Key Considerations:
 * - Assembly Instructions:  Use ONLY instructions that have the SAME binary representation
 *                           in both 32-bit and 64-bit modes.
 * - `.code64` Directive:  We're compiling with `.code64` to ensure the assembler uses
 *                         the correct 64-bit version of instructions (e.g., `inc`).
 * - Register Notation:
 *     - Use 64-bit register names (like `%rsi`) for register-indirect addressing to avoid
 *       incorrect address size prefixes.
 *     - It's safe to use `%esi` with `mov` instructions, as the high 32 bits are zeroed
 *       in 64-bit mode.
 *
 * IMPORTANT:
 * Thoroughly test ANY changes to this function in BOTH 32-bit and 64-bit boot environments.
 */

.code64
.section .text.init_page_table
.globl init_page_table
.type init_page_table, @function

init_page_table:
	mov $0x80000001, %eax
	cpuid
	test $(1 << 26), %edx
	jnz setup_1gb

setup_2mb:
	mov $2048, %edi
	mov $(_PRES + _RW + _US + _PS + _A + _D), %eax
	mov $0, %ecx
	mov $main_page_table, %esi

loop_2mb:
	movl %eax, (%rsi, %rcx, 8)
	movl $0, 4(%rsi, %rcx, 8)
	add $0x200000, %eax
	inc %ecx
	cmp %edi, %ecx
	jb loop_2mb

	mov $4, %edi
	mov $main_page_table, %eax
	add $(_PRES + _RW + _US + _A), %eax
	mov $0, %ecx
	mov $extra_page_table, %esi

fill_extra_page_table:
	movl %eax, (%rsi, %rcx, 8)
	movl $0, 4(%rsi, %rcx, 8)
	add $4096, %eax
	inc %ecx
	cmp %edi, %ecx
	jb fill_extra_page_table

	mov $extra_page_table, %eax
	jmp leave

setup_1gb:
	mov $512, %edi
	mov $(_PRES + _RW + _US + _PS + _A + _D), %eax
	mov $0, %ebx
	mov $0, %ecx
	mov $main_page_table, %esi

loop_1gb:
	mov %eax, (%rsi, %rcx, 8)
	mov %ebx, 4(%rsi, %rcx, 8)
	add $0x40000000, %eax
	cmp $0x40000000, %eax
	ja no_overflow_1gb
	inc %ebx
no_overflow_1gb:
	inc %ecx
	cmp %edi, %ecx
	jb loop_1gb

	mov $main_page_table, %eax

leave:
	or $(_PRES + _RW + _US + _A), %eax
	mov %eax, pm4le

	ret
