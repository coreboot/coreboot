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

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

#define CPU_PHYSMASK_HI  (1 << (CONFIG_CPU_ADDR_BITS - 32) - 1)

#define NoEvictMod_MSR 0x2e0

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
	/* Clean-up MTRR_DEF_TYPE_MSR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x22)
	/* Zero out all fixed range MTRRs. */
	movl	$mtrr_table, %esi
	movl	$((mtrr_table_end - mtrr_table) >> 1), %edi
	xorl	%eax, %eax
	xorl	%edx, %edx
clear_mtrrs:
	movw	(%esi), %bx
	movzx	%bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	clear_mtrrs

	/* Zero out all variable range MTRRs. */
	movl	$MTRR_CAP_MSR, %ecx
	rdmsr
	andl	$0xff, %eax
	shl	$1, %eax
	movl	%eax, %edi
	movl	$0x200, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
clear_var_mtrrs:
	wrmsr
	add	$1, %ecx
	dec	%edi
	jnz	clear_var_mtrrs

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

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* enable the 'no eviction' mode */
	movl    $NoEvictMod_MSR, %ecx
	rdmsr
	orl     $1, %eax
	andl    $~2, %eax
	wrmsr

	/* Clear the cache memory region. This will also fill up the cache. */
	movl	$CACHE_AS_RAM_BASE, %esi
	movl	%esi, %edi
	movl	$(CACHE_AS_RAM_SIZE >> 2), %ecx
	// movl	$0x23322332, %eax
	xorl	%eax, %eax
	rep	stosl

	/* enable the 'no eviction run' state */
	movl    $NoEvictMod_MSR, %ecx
	rdmsr
	orl     $3, %eax
	wrmsr

	post_code(0x26)
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
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	movl	$CPU_PHYSMASK_HI, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	post_code(0x28)
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
	post_code(0x29)
	/* Call romstage.c main function. */
	call	romstage_main
	/* Save return value from romstage_main. It contains the stack to use
	 * after cache-as-ram is torn down. It also contains the information
	 * for setting up MTRRs. */
	movl	%eax, %esp

	post_code(0x30)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(0x31)

	/* Disable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~MTRR_DEF_TYPE_EN), %eax
	wrmsr

	post_code(0x32)

	/* Disable the no eviction run state */
	movl    $NoEvictMod_MSR, %ecx
	rdmsr
	andl    $~2, %eax
	wrmsr

	invd

	/* Disable the no eviction mode */
	rdmsr
	andl    $~1, %eax
	wrmsr

	post_code(0x33)

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$~(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	post_code(0x36)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(0x38)

	/* Clear all of the variable MTRRs. */
	popl	%ebx
	movl	$MTRR_PHYS_BASE(0), %ecx
	clr	%eax
	clr	%edx

1:
	testl	%ebx, %ebx
	jz	1f
	wrmsr			/* Write MTRR base. */
	inc	%ecx
	wrmsr			/* Write MTRR mask. */
	inc	%ecx
	dec	%ebx
	jmp	1b

1:
	/* Get number of MTRRs. */
	popl	%ebx
	movl	$MTRR_PHYS_BASE(0), %ecx
2:
	testl	%ebx, %ebx
	jz	2f

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
	jmp	2b
2:

	post_code(0x39)

	/* And enable cache again after setting MTRRs. */
	movl	%cr0, %eax
	andl	$~(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	post_code(0x3a)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(0x3b)

	/* Invalidate the cache again. */
	invd

	post_code(0x3c)

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */
	call	romstage_after_car

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

mtrr_table:
	/* Fixed MTRRs */
	.word 0x250, 0x258, 0x259
	.word 0x268, 0x269, 0x26A
	.word 0x26B, 0x26C, 0x26D
	.word 0x26E, 0x26F
mtrr_table_end:
