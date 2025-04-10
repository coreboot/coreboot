/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * For starting coreboot in long mode.
 *
 * For reference see "AMD64 ArchitectureProgrammer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3
 *
 * Clobbers: eax, ecx, edx
 */

#if ENV_X86_64
	.code32
#if CONFIG(PAGE_TABLES_IN_CBFS)
#if (CONFIG_ARCH_X86_64_PGTBL_LOC & 0xfff) > 0
#error pagetables must be 4KiB aligned!
#endif
#endif

#include <cpu/x86/gdt.h>
#include <cpu/x86/msr.h>

.macro setup_longmode page_table
	/* Get page table address */
	movl	\page_table, %eax

	/* load identity mapped page tables */
	movl	%eax, %cr3

	/* enable PAE */
	movl	%cr4, %eax
	btsl	$5, %eax
	movl	%eax, %cr4

	/* enable long mode */
	movl	$(IA32_EFER), %ecx
	rdmsr
	btsl	$8, %eax
	wrmsr

	/* enable paging */
	movl	%cr0, %eax
	btsl	$31, %eax
	movl	%eax, %cr0

	/* use long jump to switch to 64-bit code segment */
	ljmp $GDT_CODE_SEG64, $jmp_addr\@

.code64
jmp_addr\@:
.endm

#endif
