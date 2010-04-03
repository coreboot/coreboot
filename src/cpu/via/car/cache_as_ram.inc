/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Eswar Nallusamy, LANL
 * Copyright (C) 2005 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007,2008 Carl-Daniel Hailfinger
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Jason Zhao <jasonzhao@viatech.com.cn> for VIA)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define CacheSize CONFIG_DCACHE_RAM_SIZE
#define CacheBase CONFIG_DCACHE_RAM_BASE


#include <cpu/x86/mtrr.h>

	/* Save the BIST result */
	movl	%eax, %ebp

CacheAsRam:

	/* disable cache */
	movl	%cr0, %eax
	orl	$(0x1<<30),%eax
	movl	%eax,%cr0
	invd

	/* Set the default memory type and enable fixed and variable MTRRs */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	/* Enable Variable and Fixed MTRRs */
	movl	$0x00000c00, %eax
	wrmsr

	/* Clear all MTRRs */
	xorl	%edx, %edx
	movl	$fixed_mtrr_msr, %esi

clear_fixed_var_mtrr:
	lodsl	(%esi), %eax
	testl	%eax, %eax
	jz	clear_fixed_var_mtrr_out

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp	clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:
	/* MTRRPhysBase */
	movl	$0x200, %ecx
	xorl	%edx, %edx
	movl	$(CacheBase|MTRR_TYPE_WRBACK),%eax
	wrmsr

	/* MTRRPhysMask */
	movl	$0x201, %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl	$0x0000000f,%edx
	movl	$(~(CacheSize-1)|0x800),%eax
	wrmsr

	/* enable write base caching so we can do execute in place
	 * on the flash rom.
	 */
	/* MTRRPhysBase */
	movl	$0x202, %ecx
	xorl	%edx, %edx
#if defined(CONFIG_TINY_BOOTBLOCK) && CONFIG_TINY_BOOTBLOCK
#define REAL_XIP_ROM_BASE AUTO_XIP_ROM_BASE
#else
#define REAL_XIP_ROM_BASE CONFIG_XIP_ROM_BASE
#endif
	movl    $REAL_XIP_ROM_BASE, %eax
	orl     $MTRR_TYPE_WRBACK, %eax
	wrmsr

	/* MTRRPhysMask */
	movl	$0x203, %ecx
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr

	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	/* Enable Variable and Fixed MTRRs */
	movl	$0x00000800, %eax
	wrmsr

	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

	/* Read the range with lodsl*/
	cld
	movl	$CacheBase, %esi
	movl	%esi, %edi
	movl	$(CacheSize>>2), %ecx
	rep	lodsl

	movl	$CacheBase, %esi
	movl	%esi, %edi
	movl	$(CacheSize >> 2), %ecx

	/* 0x5c5c5c5c is a memory test pattern.
	 * TODO: Check if everything works with the zero pattern as well. */
	/*xorl	%eax, %eax*/
	xorl	$0x5c5c5c5c,%eax
	rep	stosl

	movl	CONFIG_XIP_ROM_BASE, %esi
	movl	%esi, %edi
	movl	$(CONFIG_XIP_ROM_SIZE>>2), %ecx
	rep	lodsl

	/* The key point of this CAR code is C7 cache does not turn into
	 * "no fill" mode, which is not compatible with general CAR code.
	 */

	movl	$(CacheBase + CacheSize - 4), %eax
	movl	%eax, %esp

#ifdef CARTEST
testok:	movb $0x40,%al
	outb %al, $0x80
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$0x5c5c,%edx
	pushl %edx
	pushl %edx
	pushl %edx
	pushl %edx
	pushl %edx
	popl	%esi
	popl	%esi
	popl	%eax
	popl	%eax
	popl	%eax
	cmpl %edx,%eax
	jne stackerr
#endif

	/* Restore the BIST result */
	movl	%ebp, %eax

	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl	%eax  /* bist */
	call	stage1_main
	/* We will not go back */

fixed_mtrr_msr:
	.long	0x250, 0x258, 0x259
	.long	0x268, 0x269, 0x26A
	.long	0x26B, 0x26C, 0x26D
	.long	0x26E, 0x26F
var_mtrr_msr:
	.long	0x200, 0x201, 0x202, 0x203
	.long	0x204, 0x205, 0x206, 0x207
	.long	0x208, 0x209, 0x20A, 0x20B
	.long	0x20C, 0x20D, 0x20E, 0x20F
	.long	0x000 /* NULL, end of table */
