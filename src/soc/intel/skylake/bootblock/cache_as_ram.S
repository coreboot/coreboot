/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
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
 */

#include <cpu/x86/cache.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/post_code.h>
#include <rules.h>

#define IA32_PQR_ASSOC 0x0c8f
#define IA32_L3_MASK_1 0x0c91
#define IA32_L3_MASK_2 0x0c92
#define CACHE_INIT_VALUE 0
#define MSR_EVICT_CTL 0x2e0

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

	post_code(0x20)

	/*
	 * Use the MTRR default type MSR as a proxy for detecting INIT#.
	 * Reset the system if any known bits are set in that MSR. That is
	 * an indication of the CPU not being properly reset.
	 */
check_for_clean_reset:
	mov	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	and	$(MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN), %eax
	cmp	$0, %eax
	jz	no_reset
	/* perform soft reset */
	movw	$0xcf9, %dx
	movb	$0x06, %al
	outb	%al, %dx

no_reset:
	post_code(0x21)

	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list_size, %ebx
	xor	%eax, %eax
	xor	%edx, %edx
clear_fixed_mtrr:
	add	$-2, %ebx
	movzwl	fixed_mtrr_list(%ebx), %ecx
	wrmsr
	jnz	clear_fixed_mtrr

	post_code(0x22)

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

	post_code(0x23)

	/* Configure default memory type to uncacheable (UC) */
	mov	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	/* Clear enable bits and set default type to UC. */
	and	$~(MTRR_DEF_TYPE_MASK | MTRR_DEF_TYPE_EN | \
		 MTRR_DEF_TYPE_FIX_EN), %eax
	wrmsr

	/* Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
	 * based on the physical address size supported for this processor
	 * This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
	 *
	 * Examples:
	 *  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
	 *  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
	 */

	movl	$0x80000008, %eax 	/* Address sizes leaf */
	cpuid
	sub	$32, %al
	movzx	%al, %eax
	xorl	%esi, %esi
	bts	%eax, %esi
	dec	%esi			/* esi <- MTRR_PHYS_MASK_HIGH */

	post_code(0x24)

	/* Configure CAR region as write-back (WB) */
	mov	$MTRR_PHYS_BASE(0), %ecx
	mov	$CONFIG_DCACHE_RAM_BASE, %eax
	or	$MTRR_TYPE_WRBACK, %eax
	xor	%edx,%edx
	wrmsr

	/* Configure the MTRR mask for the size region */
	mov	$MTRR_PHYS_MASK(0), %ecx
	mov	$CONFIG_DCACHE_RAM_SIZE, %eax	/* size mask */
	dec	%eax
	not	%eax
	or	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(0x25)

	/* Enable variable MTRRs */
	mov	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	or	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable caching */
	mov	%cr0, %eax
	and	$~(CR0_CD | CR0_NW), %eax
	invd
	mov	%eax, %cr0

	/* Disable cache eviction (setup stage) */
	mov	$MSR_EVICT_CTL, %ecx
	rdmsr
	or	$0x1, %eax
	wrmsr
	post_code(0x26)

	/* Create n-way set associativity of cache */
	xorl	%edi, %edi
find_llc_subleaf:
	movl	%edi, %ecx
	movl	$0x04, %eax
	cpuid
	inc	%edi
	and	$0xe0, %al	/* EAX[7:5] = Cache Level */
	cmp	$0x60, %al	/* Check to see if it is LLC */
	jnz	find_llc_subleaf

	/*
	 * Set MSR 0xC91 IA32_L3_MASK_! = 0xE/0xFE/0xFFE/0xFFFE
	 * for 4/8/16 way of LLC
	*/
	shr	$22, %ebx
	inc	%ebx
	/* Calculate n-way associativity of LLC */
	mov	%bl, %cl

	/*
	 * Maximizing RO cacheability while locking in the CAR to a
	 * single way since that particular way won't be victim candidate
	 * for evictions.
	 * This has been done after programing LLC_WAY_MASK_1 MSR
	 * with desired LLC way as mentioned below.
	 *
	 * Hence create Code and Data Size as per request
	 * Code Size (RO) : Up to 16M
	 * Data Size (RW) : Up to 256K
	 */
	movl	$0x01, %eax
	/*
	 * LLC Ways -> LLC_WAY_MASK_1:
	 *  4: 0x000E
	 *  8: 0x00FE
	 * 12: 0x0FFE
	 * 16: 0xFFFE
	 *
	 * These MSRs contain one bit per each way of LLC
	 * - If this bit is '0' - the way is protected from eviction
	 * - If this bit is '1' - the way is not protected from eviction
	 */
	shl	%cl, %eax
	subl	$0x02, %eax
	movl	$IA32_L3_MASK_1, %ecx
	xorl	%edx, %edx
	wrmsr
	/*
	 * Set MSR 0xC92 IA32_L3_MASK_2 = 0x1
	 *
	 * For SKL SOC, data size remains 256K consistently.
	 * Hence, creating 1-way associative cache for Data
	*/
	mov	$IA32_L3_MASK_2, %ecx
	mov	$0x01, %eax
	xorl	%edx, %edx
	wrmsr
	/*
	 * Set IA32_PQR_ASSOC = 0x02
	 *
	 * Possible values:
	 * 0: Default value, no way mask should be applied
	 * 1: Apply way mask 1 to LLC
	 * 2: Apply way mask 2 to LLC
	 * 3: Shouldn't be use in NEM Mode
	 */
	movl	$IA32_PQR_ASSOC, %ecx
	movl	$0x02, %eax
	xorl	%edx, %edx
	wrmsr

	movl	$CONFIG_DCACHE_RAM_BASE, %edi
	movl	$CONFIG_DCACHE_RAM_SIZE, %ecx
	shr	$0x02, %ecx
	movl	$CACHE_INIT_VALUE, %eax
	cld
	rep	stosl
	/*
	 * Set IA32_PQR_ASSOC = 0x01
	 * At this stage we apply LLC_WAY_MASK_1 to the cache.
	 * i.e. way 0 is protected from eviction.
	*/
	movl	$IA32_PQR_ASSOC, %ecx
	movl	$0x01, %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x27)
	/*
	 * Enable No-Eviction Mode Run State by setting
	 * NO_EVICT_MODE MSR 2E0h bit [1] = '1'.
	 */

	movl	$MSR_EVICT_CTL, %ecx
	rdmsr
	orl	$0x02, %eax
	wrmsr

car_init_done:

	post_code(0x28)

	/* Setup bootblock stack */
	mov	$_car_stack_end, %esp

	post_code(0x29)

	/*push TSC value to stack*/
	movd	%mm2, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax 	/* tsc[31:0] */

before_carstage:
	post_code(0x2A)

	call	bootblock_c_entry
	/* Never reached */

.halt_forever:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.halt_forever

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
