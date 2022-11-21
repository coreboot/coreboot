/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/post_codes.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.section .init
.global bootblock_pre_c_entry

.code32
_cache_as_ram_setup:

bootblock_pre_c_entry:

cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)

	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list_size, %ebx
	xor	%eax, %eax
	xor	%edx, %edx

clear_fixed_mtrr:
	add	$-2, %ebx
	movzwl	fixed_mtrr_list(%ebx), %ecx
	wrmsr
	jnz	clear_fixed_mtrr

	/* Figure out how many MTRRs we have, and clear them out */
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
	post_code(POST_SOC_SET_DEF_MTRR_TYPE)

	/* Configure the default memory type to uncacheable. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(POST_SOC_DETERMINE_CPU_ADDR_BITS)

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

	post_code(POST_SOC_SET_CAR_BASE)

	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$_car_mtrr_start, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	xorl	%edx, %edx
	wrmsr

	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	movl	$_car_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(POST_SOC_ENABLE_MTRRS)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(POST_SOC_ENABLE_CACHE)

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* Read then clear the CAR region. This will also fill up the cache.
	 * IMPORTANT: The read is mandatory.
	 */
	cld
	movl	$_car_mtrr_start, %edi
	movl	$_car_mtrr_size, %ecx
	shr	$2, %ecx
	movl	%ecx, %ebx
	movl	%edi, %esi
	rep	lodsl
	movl	%ebx, %ecx
	xorl	%eax, %eax
	rep	stosl

	post_code(POST_SOC_DISABLE_CACHE)
	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	movl	$_program, %eax
	andl	$_xip_mtrr_mask, %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	$_xip_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(POST_SOC_FILL_CACHE)
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

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

before_c_entry:
	post_code(POST_BOOTBLOCK_BEFORE_C_ENTRY)
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

_cache_as_ram_setup_end:
