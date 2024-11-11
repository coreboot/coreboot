/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.section .init
.global bootblock_pre_c_entry

.code32
_cache_as_ram_setup:

bootblock_pre_c_entry:

cache_as_ram:
	post_code(POSTCODE_BOOTBLOCK_CAR)

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
	movl	$_car_mtrr_start, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(0), %ecx
	/* This assumes we never access addresses above 2^36 in CAR. */
	movl	$0x0000000f, %edx
	movl	$_car_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	/*
	 * Enable write base caching so we can do execute in place (XIP)
	 * on the flash ROM.
	 */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	movl	$_program, %eax
	andl	$_xip_mtrr_mask, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	movl	$0x0000000f, %edx
	movl	$_xip_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	/* Set the default memory type and enable variable MTRRs. */
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
	movl	$_car_mtrr_start, %esi
	movl	%esi, %edi
	movl	$_car_mtrr_size, %ecx
	shr	$2, %ecx
	movl    %ecx, %ebx
	rep	lodsl

	/* Zero out the cache-as-ram area. */
	movl	%ebx, %ecx
	xorl	%eax, %eax
	rep	stosl

	/*
	 * The key point of this CAR code is C7 cache does not turn into
	 * "no fill" mode, which is not compatible with general CAR code.
	 */

	/* Setup the stack. */
	mov	$_ecar_stack, %esp

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

	/* Copy .data section content to Cache-As-Ram */
#include <cpu/x86/copy_data_section.inc>

before_c_entry:
	call	bootblock_c_entry_bist

	/* Should never see this postcode */
	post_code(POSTCODE_DEAD_CODE)

.Lhlt:
	hlt
	jmp	.Lhlt
