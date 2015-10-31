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
#include <cbmem.h>

/* The full cache-as-ram size includes the cache-as-ram portion from coreboot
 * and the space used by the reference code. These 2 values combined should
 * be a power of 2 because the MTRR setup assumes that. */
#define CACHE_AS_RAM_SIZE \
	(CONFIG_DCACHE_RAM_SIZE + CONFIG_DCACHE_RAM_MRC_VAR_SIZE)
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

/* Cache all of CBFS just below 4GiB as Write-Protect type. */
#define CODE_CACHE_SIZE  (CONFIG_CBFS_SIZE)
#define CODE_CACHE_BASE  (-CODE_CACHE_SIZE)
#define CODE_CACHE_MASK  (~(CODE_CACHE_SIZE - 1))
#define CPU_PHYSMASK_HI  ((1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1)

#define NoEvictMod_MSR 0x2e0
#define BBL_CR_CTL3_MSR 0x11e
#define MCG_CAP_MSR 0x179

	/* Save the BIST result. */
	movl	%eax, %ebp

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
	movl	$MCG_CAP_MSR, %ecx
	rdmsr
	movzx	%al, %ebx
	/* First variable MTRR. */
	movl	$0x200, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	1:
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	1b

	/* Zero out all fixed range and variable range MTRRs. */
	movl	$fixed_mtrr_table, %esi
	movl	$((fixed_mtrr_table_end - fixed_mtrr_table) >> 1), %edi
	xorl	%eax, %eax
	xorl	%edx, %edx
	1:
	movw	(%esi), %bx
	movzx	%bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	1b

	post_code(0x23)
	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x24)
	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	movl	$(~(CACHE_AS_RAM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	movl	$CPU_PHYSMASK_HI, %edx
	wrmsr

	post_code(0x25)
	/* Set code caching up for romstage. */
	movl	$(MTRR_PHYS_BASE(1)), %ecx
	movl	$(CODE_CACHE_BASE | MTRR_TYPE_WRPROT), %eax
	xorl	%edx, %edx
	wrmsr

	movl	$(MTRR_PHYS_MASK(1)), %ecx
	movl	$(CODE_CACHE_MASK | MTRR_PHYS_MASK_VALID), %eax
	movl	$CPU_PHYSMASK_HI, %edx
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
	movl	$(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE), %eax
	movl	%eax, %esp

	/* Push the initial TSC value from boot block. The low 32 bits are
	 * in mm0, and the high 32 bits are in mm1. */
	movd	%mm1, %eax
	pushl	%eax
	movd	%mm0, %eax
	pushl	%eax
	/* Restore the BIST result. */
	movl	%ebp, %eax
	movl	%esp, %ebp
	pushl	%eax

before_romstage:
	post_code(0x2a)
	/* Call romstage.c main function. */
	call	romstage_main
	/* Save return value from romstage_main. It contains the stack to use
	 * after cache-as-ram is torn down. It also contains the information
	 * for setting up MTRRs. */
	movl	%eax, %ebx

	post_code(0x2b)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(0x2c)

	/* Disable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~MTRR_DEF_TYPE_EN), %eax
	wrmsr

	invd

	post_code(0x2d)

	/* Disable the no eviction run state */
	movl	$NoEvictMod_MSR, %ecx
	rdmsr
	andl	$~2, %eax
	wrmsr

	/* Disable the no eviction mode */
	rdmsr
	andl    $~1, %eax
	wrmsr

	post_code(0x2e)

	/* Setup stack as indicated by return value from ramstage_main(). */
	movl	%ebx, %esp

	/* Get number of MTRRs. */
	popl	%ebx
	movl	$MTRR_PHYS_BASE(0), %ecx
1:
	testl	%ebx, %ebx
	jz	1f

	/* Low 32 bits of MTRR base. */
	popl	%eax
	/* Upper 32 bits of MTRR base. */
	popl	%edx
	/* Write MTRR base. */
	wrmsr
	inc	%ecx
	/* Low 32 bits of MTRR mask. */
	popl	%eax
	/* Upper 32 bits of MTRR mask. */
	popl	%edx
	/* Write MTRR mask. */
	wrmsr
	inc	%ecx

	dec	%ebx
	jmp	1b
1:
	post_code(0x2f)

	/* And enable cache again after setting MTRRs. */
	movl	%cr0, %eax
	andl	$~(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	post_code(0x30)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(0x31)

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */
	call	romstage_after_car

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

/* Fixed MTRRs */
fixed_mtrr_table:
	.word 0x250, 0x258, 0x259
	.word 0x268, 0x269, 0x26A
	.word 0x26B, 0x26C, 0x26D
	.word 0x26E, 0x26F
fixed_mtrr_table_end:
