/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * For starting coreboot in long mode.
 *
 * For reference see "AMD64 ArchitectureProgrammer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3
 *
 * Clobbers: eax, ecx, edx
 */

#if defined(__x86_64__)
	.code32
#if (CONFIG_ARCH_X86_64_PGTBL_LOC & 0xfff) > 0
#error pagetables must be 4KiB aligned!
#endif

#include <cpu/x86/msr.h>
#include <arch/rom_segs.h>

setup_longmode:
	/* Get page table address */
	movl	$(CONFIG_ARCH_X86_64_PGTBL_LOC), %eax

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
	ljmp $ROM_CODE_SEG64, $__longmode_start
.code64
__longmode_start:

#endif
