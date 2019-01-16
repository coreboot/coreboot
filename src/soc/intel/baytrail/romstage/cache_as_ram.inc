/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
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
#include <cpu/x86/msr.h>

#include "fmap_config.h"

/* The full cache-as-ram size includes the cache-as-ram portion from coreboot
 * and the space used by the reference code. These 2 values combined should
 * be a power of 2 because the MTRR setup assumes that. */
#define CACHE_AS_RAM_SIZE \
	(CONFIG_DCACHE_RAM_SIZE + CONFIG_DCACHE_RAM_MRC_VAR_SIZE)
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

/* Cache all of CBFS just below 4GiB as Write-Protect type. */
#define CODE_CACHE_SIZE  _ALIGN_UP_POW2(___FMAP__COREBOOT_SIZE)
#define CODE_CACHE_BASE  (-CODE_CACHE_SIZE)
#define CODE_CACHE_MASK  (~(CODE_CACHE_SIZE - 1))

#define NoEvictMod_MSR 0x2e0
#define BBL_CR_CTL3_MSR 0x11e

cache_as_ram:
	post_code(0x20)

	/* Send INIT IPI to all excluding ourself. */
	movl	$0x000C4500, %eax
	movl	$0xFEE00300, %esi
	movl	%eax, (%esi)

	/* All CPUs need to be in Wait for SIPI state */
wait_for_sipi:
	movl	(%esi), %eax
	bt	$12, %eax
	jc	wait_for_sipi

	post_code(0x21)
	/* Configure the default memory type to uncacheable as well as disable
	 * fixed and variable range mtrrs. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(0x22)
	/* Zero the variable MTRRs. */
	movl	$IA32_MCG_CAP, %ecx
	rdmsr
	movzx	%al, %ebx
	/* First variable MTRR. */
	movl	$MTRR_PHYS_BASE(0), %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
1:
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	1b

	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list_size, %ebx
	xor	%eax, %eax
	xor	%edx, %edx

clear_fixed_mtrr:
	add	$-2, %ebx
	movzwl	fixed_mtrr_list(%ebx), %ecx
	wrmsr
	jnz	clear_fixed_mtrr

	/* Determine CPU_ADDR_BITS and load PHYSMASK high word to %edx. */
	movl	$0x80000008, %eax
	cpuid
	movb	%al, %cl
	sub	$32, %cl
	movl	$1, %edx
	shl	%cl, %edx
	subl	$1, %edx

	/* Preload high word of address mask (in %edx) for Variable
	 * MTRRs 0 and 1.
	 */
addrsize_set_high:
	xorl	%eax, %eax
	movl	$MTRR_PHYS_MASK(0), %ecx
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	wrmsr


	post_code(0x23)
	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x24)
	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	movl	$(~(CACHE_AS_RAM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	post_code(0x25)
	/* Set code caching up for romstage. */
	movl	$(MTRR_PHYS_BASE(1)), %ecx
	movl	$(CODE_CACHE_BASE | MTRR_TYPE_WRPROT), %eax
	xorl	%edx, %edx
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	$(~(CACHE_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(0x26)

	/* Enable the L2 cache. */
	movl	$BBL_CR_CTL3_MSR, %ecx
	rdmsr
	orl	$0x100, %eax
	wrmsr

	post_code(0x27)

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* enable the 'no eviction' mode */
	movl	$NoEvictMod_MSR, %ecx
	rdmsr
	orl	$1, %eax
	wrmsr

	post_code(0x28)

	/* Clear the cache memory region. This will also fill up the cache */
	movl	$CACHE_AS_RAM_BASE, %esi
	movl	%esi, %edi
	movl	$(CACHE_AS_RAM_SIZE >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	/* enable no evict mode */
	movl	$NoEvictMod_MSR, %ecx
	rdmsr
	orl	$2, %eax
	wrmsr

	post_code(0x29)

	/* Setup the stack. */
	mov	$_car_stack_end, %esp

	/* Need to align stack to 16 bytes at call instruction. Account for
	the pushes below. */
	andl	$0xfffffff0, %esp
	subl	$4, %esp

	/* push TSC and BIST to stack */
	movd	%mm0, %eax
	pushl	%eax	/* BIST */
	movd	%mm2, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax	/* tsc[31:0] */

before_c_entry:
	post_code(0x2a)
	call	bootblock_c_entry_bist

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
