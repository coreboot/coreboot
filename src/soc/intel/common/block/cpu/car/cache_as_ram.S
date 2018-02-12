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

#include <commonlib/helpers.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/post_code.h>
#include <rules.h>
#include <intelblocks/msr.h>

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
	/* perform warm reset */
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

#if ((CONFIG_DCACHE_RAM_SIZE & (CONFIG_DCACHE_RAM_SIZE - 1)) == 0)
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
	movl	%esi, %edx	/* edx <- MTRR_PHYS_MASK_HIGH */
	wrmsr
#elif (CONFIG_DCACHE_RAM_SIZE == 768 * KiB) /* 768 KiB */
	/* Configure CAR region as write-back (WB) */
	mov	$MTRR_PHYS_BASE(0), %ecx
	mov	$CONFIG_DCACHE_RAM_BASE, %eax
	or	$MTRR_TYPE_WRBACK, %eax
	xor	%edx,%edx
	wrmsr

	mov	$MTRR_PHYS_MASK(0), %ecx
	mov	$(512 * KiB), %eax	/* size mask */
	dec	%eax
	not	%eax
	or	$MTRR_PHYS_MASK_VALID, %eax
	movl	%esi, %edx	/* edx <- MTRR_PHYS_MASK_HIGH */
	wrmsr

	mov	$MTRR_PHYS_BASE(1), %ecx
	mov	$(CONFIG_DCACHE_RAM_BASE + 512 * KiB), %eax
	or	$MTRR_TYPE_WRBACK, %eax
	xor	%edx,%edx
	wrmsr

	mov	$MTRR_PHYS_MASK(1), %ecx
	mov	$(256 * KiB), %eax	/* size mask */
	dec	%eax
	not	%eax
	or	$MTRR_PHYS_MASK_VALID, %eax
	movl	%esi, %edx	/* edx <- MTRR_PHYS_MASK_HIGH */
	wrmsr
#else
#error "DCACHE_RAM_SIZE is not a power of 2 and setup code is missing"
#endif
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

#if IS_ENABLED(CONFIG_INTEL_CAR_NEM)
	jmp car_nem
#elif IS_ENABLED(CONFIG_INTEL_CAR_CQOS)
	jmp car_cqos
#elif IS_ENABLED(CONFIG_INTEL_CAR_NEM_ENHANCED)
	jmp car_nem_enhanced
#else
	jmp	.halt_forever /* In case nothing has selected */
#endif

.global car_init_done
car_init_done:

	post_code(0x29)

	/* Setup bootblock stack */
	mov	$_car_stack_end, %esp

	/* Need to align stack to 16 bytes at call instruction. Account for
	   the two pushes below. */
	andl	$0xfffffff0, %esp
	sub	$8, %esp

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

#if IS_ENABLED(CONFIG_INTEL_CAR_NEM)
.global car_nem
car_nem:
	/* Disable cache eviction (setup stage) */
	mov	$MSR_EVICT_CTL, %ecx
	rdmsr
	or	$0x1, %eax
	wrmsr

	post_code(0x26)

	/* Clear the cache memory region. This will also fill up the cache */
	movl	$CONFIG_DCACHE_RAM_BASE, %edi
	movl	$CONFIG_DCACHE_RAM_SIZE, %ecx
	shr	$0x02, %ecx
	xor	%eax, %eax
	cld
	rep	stosl

	post_code(0x27)

	/* Disable cache eviction (run stage) */
	mov	$MSR_EVICT_CTL, %ecx
	rdmsr
	or	$0x2, %eax
	wrmsr

	post_code(0x28)

	jmp car_init_done

#elif IS_ENABLED(CONFIG_INTEL_CAR_CQOS)
.global car_cqos
car_cqos:
	/*
	 * Create CBM_LEN_MASK based on CBM_LEN
	 * Get CPUID.(EAX=10H, ECX=2H):EAX.CBM_LEN[bits 4:0]
	 */
	mov $0x10, %eax
	mov $0x2,  %ecx
	cpuid
	and $0x1F, %eax
	add $1, %al

	mov $1, %ebx
	mov %al, %cl
	shl %cl, %ebx
	sub $1, %ebx

	/* Store the CBM_LEN_MASK in mm3 for later use. */
	movd %ebx, %mm3

	/*
	 * Disable both L1 and L2 prefetcher. For yet-to-understood reason,
	 * prefetchers slow down filling cache with rep stos in CQOS mode.
	 */
	mov	$MSR_PREFETCH_CTL, %ecx
	rdmsr
	or	$(PREFETCH_L1_DISABLE | PREFETCH_L2_DISABLE), %eax
	wrmsr

#if (CONFIG_DCACHE_RAM_SIZE == CONFIG_L2_CACHE_SIZE)
/*
 * If CAR size is set to full L2 size, mask is calculated as all-zeros.
 * This is not supported by the CPU/uCode.
 */
#error "CQOS CAR may not use whole L2 cache area"
#endif

	/* Calculate how many bits to be used for CAR */
	xor	%edx, %edx
	mov	$CONFIG_DCACHE_RAM_SIZE, %eax	/* dividend */
	mov	$CONFIG_CACHE_QOS_SIZE_PER_BIT, %ecx	/* divisor */
	div	%ecx		/* result is in eax */
	mov	%eax, %ecx	/* save to ecx */
	mov	$1, %ebx
	shl	%cl, %ebx
	sub	$1, %ebx	/* resulting mask is is in ebx */

	/* Set this mask for initial cache fill */
	mov	$MSR_L2_QOS_MASK(0), %ecx
	rdmsr
	mov	%ebx, %eax
	wrmsr

	/* Set CLOS selector to 0 */
	mov	$MSR_IA32_PQR_ASSOC, %ecx
	rdmsr
	and	$~IA32_PQR_ASSOC_MASK, %edx	/* select mask 0 */
	wrmsr

	/* We will need to block CAR region from evicts */
	mov	$MSR_L2_QOS_MASK(1), %ecx
	rdmsr
	/* Invert bits that are to be used for cache */
	mov	%ebx, %eax
	xor	$~0, %eax			/* invert 32 bits */

	/*
	 * Use CBM_LEN_MASK stored in mm3 to set bits based on Capacity Bit
	 * Mask Length.
	 */
	movd	%mm3, %ebx
	and	%ebx, %eax
	wrmsr

	post_code(0x26)

	/* Clear the cache memory region. This will also fill up the cache */
	movl	$CONFIG_DCACHE_RAM_BASE, %edi
	movl	$CONFIG_DCACHE_RAM_SIZE, %ecx
	shr	$0x02, %ecx
	xor	%eax, %eax
	cld
	rep	stosl

	post_code(0x27)

	/* Cache is populated. Use mask 1 that will block evicts */
	mov	$MSR_IA32_PQR_ASSOC, %ecx
	rdmsr
	and	$~IA32_PQR_ASSOC_MASK, %edx	/* clear index bits first */
	or	$1, %edx			/* select mask 1 */
	wrmsr

	/* Enable prefetchers */
	mov	$MSR_PREFETCH_CTL, %ecx
	rdmsr
	and	$~(PREFETCH_L1_DISABLE | PREFETCH_L2_DISABLE), %eax
	wrmsr

	post_code(0x28)

	jmp car_init_done

#elif IS_ENABLED(CONFIG_INTEL_CAR_NEM_ENHANCED)
.global car_nem_enhanced
car_nem_enhanced:
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
	 * This has been done after programming LLC_WAY_MASK_1 MSR
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
	movl	$MSR_IA32_L3_MASK_1, %ecx
	xorl	%edx, %edx
	wrmsr
	/*
	 * Set MSR 0xC92 IA32_L3_MASK_2 = 0x1
	 *
	 * For SKL SOC, data size remains 256K consistently.
	 * Hence, creating 1-way associative cache for Data
	*/
	mov	$MSR_IA32_L3_MASK_2, %ecx
	mov	$0x01, %eax
	xorl	%edx, %edx
	wrmsr
	/*
	 * Set MSR_IA32_PQR_ASSOC = 0x02
	 *
	 * Possible values:
	 * 0: Default value, no way mask should be applied
	 * 1: Apply way mask 1 to LLC
	 * 2: Apply way mask 2 to LLC
	 * 3: Shouldn't be use in NEM Mode
	 */
	movl	$MSR_IA32_PQR_ASSOC, %ecx
	movl	$0x02, %eax
	xorl	%edx, %edx
	wrmsr

	movl	$CONFIG_DCACHE_RAM_BASE, %edi
	movl	$CONFIG_DCACHE_RAM_SIZE, %ecx
	shr	$0x02, %ecx
	xor	%eax, %eax
	cld
	rep	stosl
	/*
	 * Set MSR_IA32_PQR_ASSOC = 0x01
	 * At this stage we apply LLC_WAY_MASK_1 to the cache.
	 * i.e. way 0 is protected from eviction.
	*/
	movl	$MSR_IA32_PQR_ASSOC, %ecx
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

	post_code(0x28)

	jmp car_init_done
#endif
