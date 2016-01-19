/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2016 Intel Corp.
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

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

/*
 * MTRR definitions
 */
.equ		IA32_MTRR_CAP,			0x00fe

.equ		NO_EVICT_MODE,			0x02e0

.equ		IA32_PQR_ASSOC,			0x0c8f
.equ		IA32_L3_MASK_1,			0x0c91
.equ		IA32_L3_MASK_2,			0x0c92
.equ		CACHE_INIT_VALUE,		0

/*
 * See BWG - chapter "Determining Cacheable Code Region Base Addresses and Ranges".
 *
 */

	movl	%edi, %ebp		/* Put BIST value in a safe place */
	/*
	 * Ensure that all variable-range MTRR valid flags are clear and
	 * IA32_MTRR_DEF_TYPE MSR E flag is clear.  Note: This is the default state
	 * after hardware reset.
	 *
	 * Initialize all fixed-range and variable-range MTRR register fields to 0.
	 */
	mov	$(MtrrByteCountFixed), %ebx		/* EBX = size of  Fixed MTRRs */

	xorl	%eax, %eax			/* Clear the low dword to write */
	xorl	%edx, %edx			/* Clear the high dword to write */
	xorl	%ecx, %ecx
	/* Clearing Fixed Range MTRRs */
clear_mtrr_fixed:
	addl	$(-2), %ebx			/* need to check it */
	movw	(MtrrInitTable)(%ebx), %cx	/* cx <- address of mtrr to zero */
	wrmsr
	jnz	clear_mtrr_fixed			/* loop through the whole table */

	post_code(0x21)

	/* Clearing Variable Range MTRRs */
	movl	$MTRR_CAP_MSR, %ecx
	rdmsr
	movzx	%al, %ebx
	clr	%eax
	clr	%edx
	movl	$MTRR_PHYS_BASE(0), %ecx
clear_var_mtrr:
	wrmsr
	inc	%ecx
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	clear_var_mtrr

	post_code(0x22)

	/*
	 * Configure the default memory type to un-cacheable (UC) in the
	 * IA32_MTRR_DEF_TYPE MSR.
	 */

	movl	$MTRR_DEF_TYPE_MSR, %ecx	/* Load the MTRR default type index */
	rdmsr
	andl	$0xFFFFF300, %eax		/* Clear the enable bits and def type UC. */
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

	/*
	 *   Configure the DataStack region as write-back (WB) cacheable memory type
	 *   using the variable range MTRRs.
	 *
	 *
	 * Set the base address of the DataStack cache range
	 */

	movl	$CONFIG_DCACHE_RAM_BASE, %eax
	orl	$MTRR_TYPE_WRBACK, %eax	/* Load the write-back cache value */
	xorl	%edx, %edx			/* clear upper dword */
	movl	$MTRR_PHYS_BASE(0), %ecx	/* Load the MTRR index */
	wrmsr					/* the value in MTRR_PHYS_BASE_0 */

	/*
	 * Set the mask for the DataStack cache range
	 * Compute MTRR mask value:  Mask = NOT (Size - 1)
	 */
	movl	$CONFIG_DCACHE_RAM_SIZE_TOTAL, %eax
	dec	%eax
	not	%eax
	orl	$MTRR_PHYS_MASK_VALID, %eax	/* turn on the Valid flag */
	movl	%esi, %edx			/* edx <- MTRR_PHYS_MASK_HIGH */
	inc	%ecx
	wrmsr					/* the value in MTRR_PHYS_MASK_0 */

	post_code(0x23)

	/*
	 * Enable the MTRRs by setting the IA32_MTRR_DEF_TYPE MSR E flag.
	 */
	movl	$MTRR_DEF_TYPE_MSR, %ecx               /* Load the MTRR default type index */
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax             /* Enable variable range MTRRs */
	wrmsr

	post_code(0x24)

	/*
	 *   Enable the logical processor's (BSP) cache: execute INVD and set
	 *   CR0.CD = 0, CR0.NW = 0.
	 */
	movl	%cr0, %eax
	and	$(~(CR0_CD + CR0_NW)), %eax
	invd
	movl	%eax, %cr0

	/*
	 *   Enable No-Eviction Mode Setup State by setting
	 *   NO_EVICT_MODE  MSR 2E0h bit [0] = '1'.
	 */
	movl	$NO_EVICT_MODE, %ecx
	rdmsr
	orl	$0x01, %eax
	wrmsr

	/* Create n-way set associativity of cache */
	xorl	%edi, %edi
Find_LLC_subleaf:
	movl	%edi, %ecx
	movl	$0x04, %eax
	cpuid
	inc	%edi
	and	$0xe0, %al	/* EAX[7:5] = Cache Level */
	cmp	$0x60, %al	/* Check to see if it is LLC */
	jnz	Find_LLC_subleaf

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
	movl	$CONFIG_DCACHE_RAM_SIZE_TOTAL, %ecx
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

	/*
	 * Enable No-Eviction Mode Run State by setting
	 * NO_EVICT_MODE MSR 2E0h bit [1] = '1'.
	 */

	movl	$NO_EVICT_MODE, %ecx
	rdmsr
	orl	$0x02, %eax
	wrmsr

	post_code(0x25)
	/*
	 *   Configure the BIOS code region as write-protected (WP) cacheable
	 *   memory type using a single variable range MTRR.
	 *
	 *   Ensure region to cache meets MTRR requirements for
	 *   size and alignment.
	 */
	movl	$(0xFFFFFFFF - CONFIG_ROM_SIZE + 1), %edi	/* Code region base */
	movl	$CONFIG_ROM_SIZE, %eax				/* Code region size */
	cmpl	$0, %edi
	jz	InvalidParameter
	cmpl	$0, %eax
	jz	InvalidParameter
	jmp	CheckPass

InvalidParameter:
	movl	$0x80000002, %eax	/* RETURN_INVALID_PARAMETER */
	jmp	.Lhlt

CheckPass:

	post_code(0x26)

	/*
	 * Program base register
	 */
	xorl	%edx, %edx			/* clear upper dword */
	movl	$MTRR_PHYS_BASE(1), %ecx	/* setup variable mtrr */
	movl	%edi, %eax
	orl	$MTRR_TYPE_WRPROT, %eax	/* set type to write protect */
	wrmsr

	movl	$CONFIG_ROM_SIZE, %eax

	/*
	 * Compute MTRR mask value:  Mask = NOT (Size - 1)
	 */
	dec	%eax	/* eax - size to cache less one byte */
	not	%eax	/* eax contains low 32 bits of mask */
	or	$MTRR_PHYS_MASK_VALID, %eax
	/*
	 * Program mask register
	 */
	movl	$MTRR_PHYS_MASK(1) , %ecx	/* setup variable mtrr */
	movl	%esi, %edx	/* edx <- MTRR_PHYS_MASK_HIGH */
	wrmsr

	post_code(0x27)

	/*
	 * edi:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */
	movl	%ebp, %edi		/* Restore BIST value */

	.section	.rodata

MtrrInitTable:
	.word      MTRR_DEF_TYPE_MSR
	.word      MTRR_FIX_64K_00000
	.word      MTRR_FIX_16K_80000
	.word      MTRR_FIX_16K_A0000
	.word      MTRR_FIX_4K_C0000
	.word      MTRR_FIX_4K_C8000
	.word      MTRR_FIX_4K_D0000
	.word      MTRR_FIX_4K_D8000
	.word      MTRR_FIX_4K_E0000
	.word      MTRR_FIX_4K_E8000
	.word      MTRR_FIX_4K_F0000
	.word      MTRR_FIX_4K_F8000

.equ MtrrByteCountFixed,  (.-MtrrInitTable)

	.previous
