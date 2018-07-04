/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Tyan (written by Yinghai Lu for Tyan)
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2012 Kyösti Mälkki <kyosti.malkki@gmail.com>
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
#include <cpu/x86/post_code.h>

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

.code32
_cache_as_ram_setup:

	/* Save the BIST result. */
	movl	%eax, %ebp

cache_as_ram:
	post_code(0x20)

	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list_size, %ebx
	xor	%eax, %eax
	xor	%edx, %edx

clear_fixed_mtrr:
	add	$-2, %ebx
	movzwl	fixed_mtrr_list(%ebx), %ecx
	wrmsr
	jnz	clear_fixed_mtrr

	/* Figure put how many MTRRs we have, and clear them out */
	mov	$MTRR_CAP_MSR, %ecx
	rdmsr
	movzb	%al, %ebx		/* Number of variable MTRRs */
	mov	$MTRR_PHYS_BASE(0), %ecx
	xor	%eax, %eax
	xor	%edx, %edx

clear_var_mtrr:
	wrmsr
	inc	%ecx
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	clear_var_mtrr
	post_code(0x21)

	/* Configure the default memory type to uncacheable. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(0x22)

	/* Determine CPU_ADDR_BITS and load PHYSMASK high word to %edx. */
	movl	$1, %eax
	cpuid
	andl	$(1 << 6 | 1 << 17), %edx	/* PAE or PSE36 */
	jz	addrsize_set_high
	movl	$0x0f, %edx

	/* Preload high word of address mask (in %edx) for Variable
	   MTRRs 0 and 1. */
addrsize_set_high:
	xorl	%eax, %eax
	movl	$MTRR_PHYS_MASK(0), %ecx
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	wrmsr

	post_code(0x2a)

	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	movl	$(~(CACHE_AS_RAM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	post_code(0x2b)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(0x2c)

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* Read then clear the CAR region. This will also fill up the cache.
	 * IMPORTANT: The read is mandatory.
	 */
	movl	$CACHE_AS_RAM_BASE, %esi
	movl	%esi, %edi
	cld
	movl	$(CACHE_AS_RAM_SIZE >> 2), %ecx
	rep	lodsl
	movl	$(CACHE_AS_RAM_SIZE >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	post_code(0x2d)
	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * https://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$_program, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	post_code(0x2e)
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Setup the stack. */
	movl	$(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE), %eax
	movl	%eax, %esp

	/* Restore the BIST result. */
	movl	%ebp, %eax
	movl	%esp, %ebp
	pushl	%eax

before_romstage:
	post_code(0x2f)
	/* Call romstage.c main function. */
	call	romstage_main

	/* Should never see this postcode */
	post_code(POST_DEAD_CODE)

.Lhlt:
	hlt
	jmp	.Lhlt

fixed_mtrr_list:
	.word	MTRR_FIX_64K_00000
	.word	MTRR_FIX_16K_80000
	.word	MTRR_FIX_16K_A0000
	.word	MTRR_FIX_4K_C0000
	.word	MTRR_FIX_4K_C8000
	.word	MTRR_FIX_4K_D0000
	.word	MTRR_FIX_4K_D8000
	.word	MTRR_FIX_4K_E0000
	.word	MTRR_FIX_4K_E8000
	.word	MTRR_FIX_4K_F0000
	.word	MTRR_FIX_4K_F8000
fixed_mtrr_list_size = . - fixed_mtrr_list

_cache_as_ram_setup_end:
