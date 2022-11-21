/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/post_codes.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

#define NoEvictMod_MSR 0x2e0
#define BBL_CR_CTL3_MSR 0x11e

.section .init
.global bootblock_pre_c_entry

#include <cpu/intel/car/cache_as_ram_symbols.inc>

.code32
_cache_as_ram_setup:

bootblock_pre_c_entry:
	movl	$cache_as_ram, %esp /* return address */
	jmp	check_mtrr /* Check if CPU properly reset */

cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)

	/* Send INIT IPI to all excluding ourself. */
	movl	$0x000C4500, %eax
	movl	$0xFEE00300, %esi
	movl	%eax, (%esi)

	/* All CPUs need to be in Wait for SIPI state */
wait_for_sipi:
	movl	(%esi), %eax
	bt	$12, %eax
	jc	wait_for_sipi

	post_code(POST_SOC_SET_DEF_MTRR_TYPE)
	/* Clean-up MTRR_DEF_TYPE_MSR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	post_code(POST_SOC_CLEAR_FIXED_MTRRS)
	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list_size, %ebx
	xor	%eax, %eax
	xor	%edx, %edx

clear_fixed_mtrr:
	add	$-2, %ebx
	movzwl	fixed_mtrr_list(%ebx), %ecx
	wrmsr
	jnz	clear_fixed_mtrr

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

	post_code(POST_SOC_SET_MTRR_BASE)
	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	car_mtrr_start, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	xorl	%edx, %edx
	wrmsr

	post_code(POST_SOC_SET_MTRR_MASK)
	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	mov	car_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	mov	rom_mtrr_base, %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	mov	rom_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(POST_SOC_ENABLE_MTRRS)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

#if CONFIG(CPU_HAS_L2_ENABLE_MSR)
	/*
	 * Enable the L2 cache. Currently this assumes that this
	 * only affect socketed CPU's for which this is always valid,
	 * hence the static preprocesser.
	 */
	movl	$BBL_CR_CTL3_MSR, %ecx
	rdmsr
	orl	$0x100, %eax
	wrmsr
#endif

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

#if CONFIG(MICROCODE_UPDATE_PRE_RAM)
update_microcode:
	/* put the return address in %esp */
	movl	$end_microcode_update, %esp
	jmp	update_bsp_microcode
end_microcode_update:
#endif
	/* Disable caching to change MTRR's. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Clear the mask valid to disable the MTRR */
	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	andl	$(~MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* enable the 'no eviction' mode */
	movl	$NoEvictMod_MSR, %ecx
	rdmsr
	orl	$1, %eax
	andl	$~2, %eax
	wrmsr

	/* Clear the cache memory region. This will also fill up the cache. */
	cld
	xorl	%eax, %eax
	movl	$_car_mtrr_start, %edi
	movl	$_car_mtrr_size, %ecx
	shr	$2, %ecx
	rep	stosl

	/* enable the 'no eviction run' state */
	movl	$NoEvictMod_MSR, %ecx
	rdmsr
	orl	$3, %eax
	wrmsr

	post_code(POST_SOC_DISABLE_CACHE)
	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(POST_SOC_ENABLE_CACHE)
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Setup the stack. */
	mov	$_ecar_stack, %esp

	/* Need to align stack to 16 bytes at call instruction. Account for
	the pushes below. */
	andl	$0xfffffff0, %esp

#if ENV_X86_64

	#include <cpu/x86/64bit/entry64.inc>

	movd	%mm2, %rdi
	shlq	$32, %rdi
	movd	%mm1, %rsi
	or	%rsi, %rdi
	movd	%mm0, %rsi

#else
	subl	$4, %esp
	/* push TSC and BIST to stack */
	movd	%mm0, %eax
	pushl	%eax	/* BIST */
	movd	%mm2, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax	/* tsc[31:0] */
#endif

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
