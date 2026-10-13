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

#define _PRES (1ULL << 0)
#define _RW   (1ULL << 1)
#define _US   (1ULL << 2)
#define _A    (1ULL << 5)
#define _D    (1ULL << 6)
#define _PS   (1ULL << 7)

/* Page Map Level 4: Just a single 512GB pointer at PDPT */
.section .bss.pml4e
.global pml4e
.align 4096
pml4e:
.skip 4096

/* Page Directory Pointer Table: 1GB pointers at PD for first 4GB, then optionally
   (if 1GB paging is supported) 1GB pages for next 508GB (otherwise unmapped) */
.section .bss.pdpt
.global pdpt
.align 4096
pdpt:
.skip 4096

/* Page Directory: 2MB huge pages, except for one pointer to guard PT */
.section .bss.page_directories
.global page_directories
.align 4096
page_directories:
.skip 16384

/* Page Table: 4K pages, with the one covering _guard unmapped */
.section .bss.guard_page_table
.global guard_page_table
.align 4096
guard_page_table:
.skip 4096

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
	/* Check for 1GB page support first, since CPUID clobbers %ecx and %ebx.
	   %edx is preserved through the 2MB setup below. */
	mov $0x80000001, %eax
	cpuid

	/* Set up 2MB entries in page directory first (covering first 4GB) */
	mov $2048, %edi
	mov $(_PRES + _RW + _US + _PS + _A + _D), %eax
	mov $0, %ecx
	mov $page_directories, %esi

loop_page_directories:
	movl %eax, (%rsi, %rcx, 8)
	add $0x200000, %eax
	inc %ecx
	cmp %edi, %ecx
	jb loop_page_directories

	/* Set up 1GB PDPT entries. First to the page directories created above... */
	mov $4, %edi
	mov $page_directories, %eax
	or $(_PRES + _RW + _US + _A), %eax
	mov $0, %ecx
	mov $pdpt, %esi

loop_pdpt_for_pd:
	movl %eax, (%rsi, %rcx, 8)
	add $4096, %eax
	inc %ecx
	cmp %edi, %ecx
	jb loop_pdpt_for_pd

	/* ...then we check if we can support 1GB pages (via EDX from CPUID above). */
	test $(1 << 26), %edx
	jz skip_1gb_pdpt

	/* If we can, fill with direct 1GB mappings (otherwise, entries remain 0/unmapped). */
	mov $512, %edi
	mov $(_PRES + _RW + _US + _PS + _A + _D), %eax
	mov $1, %ebx

loop_1gb_pdpt:
	mov %eax, (%rsi, %rcx, 8)
	mov %ebx, 4(%rsi, %rcx, 8)
	add $0x40000000, %eax
	cmp $0x40000000, %eax
	ja no_overflow_1gb
	inc %ebx
no_overflow_1gb:
	inc %ecx
	cmp %edi, %ecx
	jb loop_1gb_pdpt

	/* Finally, build a 4K page table for the 2MB range covering _guard. */
skip_1gb_pdpt:
	mov $512, %edi
	mov $_guard, %eax
	and $0xFFE00000, %eax
	or $(_PRES + _RW + _US + _A + _D), %eax
	mov $0, %ecx
	mov $guard_page_table, %esi

loop_guard_pt:
	movl %eax, (%rsi, %rcx, 8)
	add $4096, %eax
	inc %ecx
	cmp %edi, %ecx
	jb loop_guard_pt

	/* Unmap the 4K page covering _guard. */
	mov $_guard, %ecx
	shr $12, %ecx
	and $511, %ecx
	movl $0, (%rsi, %rcx, 8)

	/* Point the PDE covering _guard to the 4K page table. */
	mov $_guard, %ecx
	shr $21, %ecx
	mov $page_directories, %esi
	mov $guard_page_table, %eax
	or $(_PRES + _RW + _US + _A), %eax
	movl %eax, (%rsi, %rcx, 8)

	/* Last step, set up the PML4: just a single entry pointed at the PDPT. */
	mov $pdpt, %eax
	or $(_PRES + _RW + _US + _A), %eax
	mov %eax, pml4e

	ret
