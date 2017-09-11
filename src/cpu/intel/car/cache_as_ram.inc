/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000, 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Eswar Nallusamy, LANL
 * Copyright (C) 2005 Tyan (written by Yinghai Lu for Tyan)
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/post_code.h>

	/* Save the BIST result. */
	movl	%eax, %ebp

CacheAsRam:
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

/*
 * 0x06 is the WB IO type for a given 4k segment.
 * segs is the number of 4k segments in the area of the particular
 *      register we want to use for CAR.
 * reg  is the register where the IO type should be stored.
 */
.macro extractmask segs, reg
.if \segs <= 0
	/*
	 * The xorl here is superfluous because at the point of first execution
	 * of this macro, %eax and %edx are cleared. Later invocations of this
	 * macro will have a monotonically increasing segs parameter.
	 */
	xorl \reg, \reg
.elseif \segs == 1
	movl	$0x06000000, \reg /* WB IO type */
.elseif \segs == 2
	movl	$0x06060000, \reg /* WB IO type */
.elseif \segs == 3
	movl	$0x06060600, \reg /* WB IO type */
.elseif \segs >= 4
	movl	$0x06060606, \reg /* WB IO type */
.endif
.endm

/*
 * carsize is the cache size in bytes we want to use for CAR.
 * windowoffset is the 32k-aligned window into CAR size.
 */
.macro simplemask carsize, windowoffset
	.set gas_bug_workaround,(((\carsize - \windowoffset) >> 12) - 4)
	extractmask gas_bug_workaround, %eax
	.set gas_bug_workaround,(((\carsize - \windowoffset) >> 12))
	extractmask gas_bug_workaround, %edx
	/*
	 * Without the gas bug workaround, the entire macro would consist
	 * only of the two lines below:
	 *   extractmask (((\carsize - \windowoffset) >> 12) - 4), %eax
	 *   extractmask (((\carsize - \windowoffset) >> 12)), %edx
	 */
.endm

#if CONFIG_DCACHE_RAM_SIZE > 0x10000
#error Invalid CAR size, must be at most 64k.
#endif
#if CONFIG_DCACHE_RAM_SIZE < 0x1000
#error Invalid CAR size, must be at least 4k. This is a processor limitation.
#endif
#if (CONFIG_DCACHE_RAM_SIZE & (0x1000 - 1))
#error Invalid CAR size, is not a multiple of 4k. This is a processor limitation.
#endif

#if CONFIG_DCACHE_RAM_SIZE > 0x8000
	/* Enable caching for 32K-64K using fixed MTRR. */
	movl	$MTRR_FIX_4K_C0000, %ecx
	simplemask CONFIG_DCACHE_RAM_SIZE, 0x8000
	wrmsr
#endif

	/* Enable caching for 0-32K using fixed MTRR. */
	movl	$MTRR_FIX_4K_C8000, %ecx
	simplemask CONFIG_DCACHE_RAM_SIZE, 0
	wrmsr

	/* Enable cache for our code in Flash because we do XIP here. */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * https://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Read the CAR region. This will also fill up the cache.
	 * IMPORTANT: This step is mandatory.
	 */
	movl	$CONFIG_DCACHE_RAM_BASE, %esi
	cld
	movl	$(CONFIG_DCACHE_RAM_SIZE >> 2), %ecx
	rep	lodsl

	/* Clear the CAR region. */
	movl	$CONFIG_DCACHE_RAM_BASE, %edi
	movl	$(CONFIG_DCACHE_RAM_SIZE >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	movl	$(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE), %eax
	movl	%eax, %esp
lout:
	/* Restore the BIST result. */
	movl	%ebp, %eax

	pushl	%eax  /* BIST */
	call	romstage_main

	/* Setup stack as indicated by return value from romstage_main(). */
	movl	%eax, %esp

	/* We don't need CAR from now on. */

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Clear the fixed MTRR we used. */
	movl	$MTRR_FIX_4K_C8000, %ecx
	xorl	%edx, %edx
	xorl	%eax, %eax
	wrmsr

#if CONFIG_DCACHE_RAM_SIZE > 0x8000
	movl	$MTRR_FIX_4K_C0000, %ecx
	wrmsr
#endif

	/*
	 * Enable variable and disable fixed MTRRs.
	 * Default memory type will be UC.
	 */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%edx, %edx
	movl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */
	call	copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt
