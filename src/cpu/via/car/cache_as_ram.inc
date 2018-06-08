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
 */

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
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN), %eax
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
	.long	MTRR_FIX_64K_00000
	.long	MTRR_FIX_16K_80000
	.long	MTRR_FIX_16K_A0000
	.long	MTRR_FIX_4K_C0000
	.long	MTRR_FIX_4K_C8000
	.long	MTRR_FIX_4K_D0000
	.long	MTRR_FIX_4K_D8000
	.long	MTRR_FIX_4K_E0000
	.long	MTRR_FIX_4K_E8000
	.long	MTRR_FIX_4K_F0000
	.long	MTRR_FIX_4K_F8000

	/* var MTRR MSRs */
	.long	MTRR_PHYS_BASE(0)
	.long	MTRR_PHYS_MASK(0)
	.long	MTRR_PHYS_BASE(1)
	.long	MTRR_PHYS_MASK(1)
	.long	MTRR_PHYS_BASE(2)
	.long	MTRR_PHYS_MASK(2)
	.long	MTRR_PHYS_BASE(3)
	.long	MTRR_PHYS_MASK(3)
	.long	MTRR_PHYS_BASE(4)
	.long	MTRR_PHYS_MASK(4)
	.long	MTRR_PHYS_BASE(5)
	.long	MTRR_PHYS_MASK(5)
	.long	MTRR_PHYS_BASE(6)
	.long	MTRR_PHYS_MASK(6)
	.long	MTRR_PHYS_BASE(7)
	.long	MTRR_PHYS_MASK(7)

	.long	0x000 /* NULL, end of table */

clear_fixed_var_mtrr_out:
	movl	$MTRR_PHYS_BASE(0), %ecx
	xorl	%edx, %edx
	movl	$(CacheBase | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(0), %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl	$0x0000000f, %edx
	movl	$(~(CacheSize - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/*
	 * Enable write base caching so we can do execute in place (XIP)
	 * on the flash ROM.
	 */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * https://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Set the default memory type and enable fixed and variable MTRRs. */
	/* TODO: Or also enable fixed MTRRs? Bug in the code? */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRR_DEF_TYPE_EN), %eax
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

	/* Zero out the cache-as-ram area. */
	xorl	%eax, %eax
	rep	stosl

	/*
	 * The key point of this CAR code is C7 cache does not turn into
	 * "no fill" mode, which is not compatible with general CAR code.
	 */

	movl	$(CacheBase + CacheSize - 4), %eax
	movl	%eax, %esp

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
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRR_DEF_TYPE_EN), %eax
	wrmsr

	/* Enable caching for 0..CACHE_TMP_RAMTOP. */
	movl	$MTRR_PHYS_BASE(0), %ecx
	xorl	%edx, %edx
	movl	$(0x0 | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(0), %ecx
	movl	$0x0000000f, %edx	/* AMD 40 bit 0xff */
	movl	$(~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Cache XIP_ROM area to speedup coreboot code. */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * https://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	xorl	%edx, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0
	invd

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	movl	$CONFIG_RAMTOP, %esp
	movl	%esp, %ebp
	call	copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt
