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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cpu/x86/stack.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <console/post_codes.h>

#define CacheSize		CONFIG_DCACHE_RAM_SIZE
#define CacheBase		CONFIG_DCACHE_RAM_BASE

	/* Save the BIST result. */
	movl	%eax, %ebp

CacheAsRam:

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0
	invd

	/* Set the default memory type and enable fixed and variable MTRRs. */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn | MTRRdefTypeFixEn), %eax
	wrmsr

	/* Clear all MTRRs. */
	xorl	%edx, %edx
	movl	$all_mtrr_msrs, %esi

clear_fixed_var_mtrr:
	lodsl	(%esi), %eax
	testl	%eax, %eax
	jz	clear_fixed_var_mtrr_out

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp	clear_fixed_var_mtrr

all_mtrr_msrs:
	/* fixed MTRR MSRs */
	.long	MTRRfix64K_00000_MSR
	.long	MTRRfix16K_80000_MSR
	.long	MTRRfix16K_A0000_MSR
	.long	MTRRfix4K_C0000_MSR
	.long	MTRRfix4K_C8000_MSR
	.long	MTRRfix4K_D0000_MSR
	.long	MTRRfix4K_D8000_MSR
	.long	MTRRfix4K_E0000_MSR
	.long	MTRRfix4K_E8000_MSR
	.long	MTRRfix4K_F0000_MSR
	.long	MTRRfix4K_F8000_MSR

	/* var MTRR MSRs */
	.long	MTRRphysBase_MSR(0)
	.long	MTRRphysMask_MSR(0)
	.long	MTRRphysBase_MSR(1)
	.long	MTRRphysMask_MSR(1)
	.long	MTRRphysBase_MSR(2)
	.long	MTRRphysMask_MSR(2)
	.long	MTRRphysBase_MSR(3)
	.long	MTRRphysMask_MSR(3)
	.long	MTRRphysBase_MSR(4)
	.long	MTRRphysMask_MSR(4)
	.long	MTRRphysBase_MSR(5)
	.long	MTRRphysMask_MSR(5)
	.long	MTRRphysBase_MSR(6)
	.long	MTRRphysMask_MSR(6)
	.long	MTRRphysBase_MSR(7)
	.long	MTRRphysMask_MSR(7)

	.long	0x000 /* NULL, end of table */

clear_fixed_var_mtrr_out:
	movl	$MTRRphysBase_MSR(0), %ecx
	xorl	%edx, %edx
	movl	$(CacheBase | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRRphysMask_MSR(0), %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl	$0x0000000f, %edx
	movl	$(~(CacheSize - 1) | MTRRphysMaskValid), %eax
	wrmsr

	/*
	 * Enable write base caching so we can do execute in place (XIP)
	 * on the flash ROM.
	 */
	movl	$MTRRphysBase_MSR(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * http://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	wrmsr

	/* Set the default memory type and enable fixed and variable MTRRs. */
	/* TODO: Or also enable fixed MTRRs? Bug in the code? */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn), %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Read the range with lodsl. */
	cld
	movl	$CacheBase, %esi
	movl	%esi, %edi
	movl	$(CacheSize >> 2), %ecx
	rep	lodsl

	movl	$CacheBase, %esi
	movl	%esi, %edi
	movl	$(CacheSize >> 2), %ecx

	/*
	 * 0x5c5c5c5c is a memory test pattern.
	 * TODO: Check if everything works with the zero pattern as well.
	 */
	/* xorl	%eax, %eax */
	xorl	$0x5c5c5c5c, %eax
	rep	stosl

#ifdef CARTEST
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * http://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %esi
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %ei
	movl	%esi, %edi
	movl	$(CONFIG_XIP_ROM_SIZE >> 2), %ecx
	rep	lodsl
#endif

	/*
	 * The key point of this CAR code is C7 cache does not turn into
	 * "no fill" mode, which is not compatible with general CAR code.
	 */

	movl	$(CacheBase + CacheSize - 4), %eax
	movl	%eax, %esp

#ifdef CARTEST
testok:
	post_code(0x40)
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$0x5c5c, %edx
	pushl	%edx
	pushl	%edx
	pushl	%edx
	pushl	%edx
	pushl	%edx
	popl	%esi
	popl	%esi
	popl	%eax
	popl	%eax
	popl	%eax
	cmpl	%edx, %eax
	jne	stackerr
#endif

	/* Restore the BIST result. */
	movl	%ebp, %eax

	/* We need to set EBP? No need. */
	movl	%esp, %ebp
	pushl	%eax	/* BIST */
	call	main

	/*
	 * TODO: Backup stack in CACHE_AS_RAM into MMX and SSE and after we
	 *       get STACK up, we restore that. It is only needed if we
	 *       want to go back.
	 */

	/* We don't need CAR from now on. */

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Set the default memory type and enable variable MTRRs. */
	/* TODO: Or also enable fixed MTRRs? Bug in the code? */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn), %eax
	wrmsr

	/* Enable caching for CONFIG_RAMBASE..CONFIG_RAMTOP. */
	movl	$MTRRphysBase_MSR(0), %ecx
	xorl	%edx, %edx
	movl	$(CONFIG_RAMBASE | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRRphysMask_MSR(0), %ecx
	movl	$0x0000000f, %edx	/* AMD 40 bit 0xff */
	movl	$(~(CONFIG_RAMTOP - CONFIG_RAMBASE - 1) | MTRRphysMaskValid), %eax
	wrmsr

	/* Cache XIP_ROM area to speedup coreboot code. */
	movl	$MTRRphysBase_MSR(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * http://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRRphysMask_MSR(1), %ecx
	xorl	%edx, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0
	invd

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	movl	$ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	call	copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

