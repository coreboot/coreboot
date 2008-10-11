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

/* Init code - Switch CPU to protected mode and enable Cache-as-Ram (CAR). */

#include <macros.h>

#define ROM_CODE_SEG 0x08
#define ROM_DATA_SEG 0x10

#define CACHE_RAM_CODE_SEG 0x18
#define CACHE_RAM_DATA_SEG 0x20

	.align  4
	.globl protected_stage0
protected_stage0:
	/* This code was used by v2. TODO. */
	lgdt	%cs:gdtptr
	ljmp	$ROM_CODE_SEG, $__protected_stage0

.globl __protected_stage0
__protected_stage0:
	/* Save the BIST result. */
	movl	%eax, %ebp

	port80_post(0x01)

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Restore the BIST value to %eax. */
	movl	%ebp, %eax

.align 4

#define	CacheSize CONFIG_CARSIZE
#define	CacheBase CONFIG_CARBASE

#define ASSEMBLY
#include <mtrr.h>

	/* Save the BIST result */
	movl    %eax, %ebp

CacheAsRam:

	/* disable cache */
	movl    %cr0, %eax
	orl    $(0x1<<30),%eax
	movl    %eax,%cr0
	invd

	/* Set the default memory type and enable fixed and variable MTRRs */
	movl    $MTRRdefType_MSR, %ecx
	xorl    %edx, %edx
	/* Enable Variable and Fixed MTRRs */
	movl    $0x00000c00, %eax
	wrmsr

	/* Clear all MTRRs */
	xorl    %edx, %edx
	movl    $fixed_mtrr_msr, %esi

clear_fixed_var_mtrr:
	lodsl   (%esi), %eax
	testl   %eax, %eax
	jz      clear_fixed_var_mtrr_out

	movl    %eax, %ecx
	xorl    %eax, %eax
	wrmsr

	jmp     clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:
	/* MTRRPhysBase */
	movl    $0x200, %ecx
	xorl    %edx, %edx
	movl    $(CacheBase|MTRR_TYPE_WRBACK),%eax
	wrmsr

	/* MTRRPhysMask */
	movl    $0x201, %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl    $0x0000000f,%edx
	movl    $(~(CacheSize-1)|0x800),%eax
	wrmsr

#if defined(CONFIG_XIP_ROM_SIZE) && defined(CONFIG_XIP_ROM_BASE)
        /* enable write base caching so we can do execute in place
         * on the flash rom.
         */
	/* MTRRPhysBase */
	movl    $0x202, %ecx
	xorl    %edx, %edx
	movl    $(XIP_ROM_BASE|MTRR_TYPE_WRBACK),%eax
	wrmsr

	/* MTRRPhysMask */
	movl    $0x203, %ecx
	movl    $0x0000000f,%edx
	movl    $(~(XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

	movl    $MTRRdefType_MSR, %ecx
	xorl    %edx, %edx
	/* Enable Variable and Fixed MTRRs */
	movl    $0x00000800, %eax
	wrmsr

	/* enable cache */
	movl    %cr0, %eax
	andl    $0x9fffffff,%eax
	movl    %eax, %cr0

	/* Read the range with lodsl */
	movl    $CacheBase, %esi
	cld
	movl    $(CacheSize >> 2), %ecx
	rep     lodsl

	/* Clear the range */
	movl    $CacheBase, %edi
	movl    $(CacheSize >> 2), %ecx
	xorl    %eax, %eax
	rep     stosl

#if defined(CONFIG_XIP_ROM_SIZE) && defined(CONFIG_XIP_ROM_BASE)
	/* Read the XIP area */
	movl    XIP_ROM_BASE, %esi
	movl    $(XIP_ROM_SIZE>>2), %ecx
	rep     lodsl
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

	/* The key point of this CAR code is C7 cache does not turn into
	 * "no fill" mode, which is not compatible with general CAR code.
	 */

	/* set up the stack pointer */
	movl    $(CacheBase + CacheSize - 4), %eax
	movl    %eax, %esp

	/* Load a different set of data segments */
	movw    $CACHE_RAM_DATA_SEG, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss

lout:
	/* Store zero for the pointer to the global variables. */
	pushl   $0

	/* Restore the BIST result. */
	movl	%ebp, %eax

	/* We need to set ebp? No need. */
	movl	%esp, %ebp

	/* Second parameter: init_detected */
	/* Store zero for the unused init_detected parameter. */
	pushl	$0
	/* First parameter: bist */
	pushl	%eax
	call	stage1_main
	/* We will not go back. */

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

#include "../stage0_common.S"
