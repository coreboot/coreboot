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

#include <cpu/x86/car.h>
#include <cpu/x86/stack.h>
#include <cpu/x86/mtrr.h>

#define CacheSize		CONFIG_DCACHE_RAM_SIZE
#define CacheBase		CONFIG_DCACHE_RAM_BASE

	save_bist_result()

CacheAsRam:

	disable_cache()
	invd

	/* Set the default memory type and enable fixed and variable MTRRs. */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn | MTRRdefTypeFixEn), %eax
	wrmsr

	/* Clear all MTRRs. */
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

clear_fixed_var_mtrr_out:
	movl	$MTRRphysBase_MSR(0), %ecx
	xorl	%edx, %edx
	movl	$(CacheBase | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRRphysMask_MSR(0), %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl	$0x0000000f, %edx
	movl	$(~(CacheSize - 1) | 0x800), %eax
	wrmsr

#if defined(CONFIG_TINY_BOOTBLOCK) && CONFIG_TINY_BOOTBLOCK
#define REAL_XIP_ROM_BASE AUTO_XIP_ROM_BASE
#else
#define REAL_XIP_ROM_BASE CONFIG_XIP_ROM_BASE
#endif

	/*
	 * Enable write base caching so we can do execute in place (XIP)
	 * on the flash ROM.
	 */
	movl	$MTRRphysBase_MSR(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The two lines below can _not_ be written like this:
	 *   movl $(REAL_XIP_ROM_BASE | MTRR_TYPE_WRBACK), %eax
	 * http://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$REAL_XIP_ROM_BASE, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr

	/* Set the default memory type and enable fixed and variable MTRRs. */
	/* TODO: Or also enable fixed MTRRs? Bug in the code? */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn), %eax
	wrmsr

	enable_cache()

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
	movl	REAL_XIP_ROM_BASE, %esi
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

	restore_bist_result()

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

	disable_cache()

	/* Set the default memory type and enable variable MTRRs. */
	/* TODO: Or also enable fixed MTRRs? Bug in the code? */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn), %eax
	wrmsr

	/* Enable caching for first 1M using variable MTRR. */
	movl	$MTRRphysBase_MSR(0), %ecx
	xorl	%edx, %edx
	movl	$(0 | MTRR_TYPE_WRBACK), %eax
	wrmsr

	/*
	 * Enable cache for 0-7ffff, 80000-9ffff, e0000-fffff;
	 * If 1M cacheable, then when S3 resume, there is stange color on
	 * screen for 2 sec. Suppose problem of a0000-dfffff and cache.
	 * And in x86_setup_fixed_mtrrs()(mtrr.c), 0-256M is set cacheable.
	 */

	movl	$MTRRphysMask_MSR(0), %ecx
	movl	$0x0000000f, %edx	/* AMD 40 bit 0xff */
	movl	$((~((0 + 0x80000) - 1)) | 0x800), %eax
	wrmsr

	movl	$MTRRphysBase_MSR(1), %ecx
	xorl	%edx, %edx
	movl	$(0x80000 | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$0x0000000f, %edx	/* AMD 40 bit 0xff */
	movl	$((~((0 + 0x20000) - 1)) | 0x800), %eax
	wrmsr

	movl	$MTRRphysBase_MSR(2), %ecx
	xorl	%edx, %edx
	movl	$(0xc0000 | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRRphysMask_MSR(2), %ecx
	movl	$0x0000000f, %edx	/* AMD 40 bit 0xff */
	movl	$((~(( 0 + 0x40000) - 1)) | 0x800), %eax
	wrmsr

	/* Cache XIP_ROM_BASE-SIZE to speedup coreboot code. */
	movl	$MTRRphysBase_MSR(3), %ecx
	xorl	%edx, %edx
	movl	$REAL_XIP_ROM_BASE, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRRphysMask_MSR(3), %ecx
	xorl	%edx, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr

	enable_cache()
	invd

	/* Clear boot_complete flag. */
	xorl	%ebp, %ebp
__main:
	post_code(0x11)
	cld			/* Clear direction flag. */

	movl	%ebp, %esi

	movl	$ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	pushl	%esi
	call	copy_and_run

.Lhlt:
	post_code(0xee)
	hlt
	jmp	.Lhlt

