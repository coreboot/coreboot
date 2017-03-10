/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_def.h>
#include <commonlib/helpers.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>
#include <soc/cpu.h>

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

.global cache_as_ram
cache_as_ram:
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
	mov	$~(CONFIG_DCACHE_RAM_SIZE - 1), %eax	/* size mask */
	or	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr
#elif (CONFIG_DCACHE_RAM_SIZE == 768 * KiB) /* 768 KiB */
	mov	$MTRR_PHYS_BASE(0), %ecx
	mov	$CONFIG_DCACHE_RAM_BASE, %eax
	or	$MTRR_TYPE_WRBACK, %eax
	xor	%edx,%edx
	wrmsr

	mov	$MTRR_PHYS_MASK(0), %ecx
	mov	$~(512 * KiB - 1), %eax	/* size mask */
	or	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	mov	$MTRR_PHYS_BASE(1), %ecx
	mov	$(CONFIG_DCACHE_RAM_BASE + 512 * KiB), %eax
	or	$MTRR_TYPE_WRBACK, %eax
	xor	%edx,%edx
	wrmsr

	mov	$MTRR_PHYS_MASK(1), %ecx
	mov	$~(256 * KiB - 1), %eax	/* size mask */
	or	$MTRR_PHYS_MASK_VALID, %eax
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

#if IS_ENABLED(CONFIG_CAR_NEM)
	/* Disable cache eviction (setup stage) */
	mov	$MSR_EVICT_CTL, %ecx
	rdmsr
	or	$0x1, %eax
	wrmsr
#else
	/*
	 * Disable both L1 and L2 prefetcher. For yet-to-understood reason,
	 * prefetchers slow down filling cache with rep stos in CQOS mode.
	 */
	mov	$MSR_PREFETCH_CTL, %ecx
	rdmsr
	or	$(PREFETCH_L1_DISABLE | PREFETCH_L2_DISABLE), %eax
	wrmsr
#endif

#if IS_ENABLED(CONFIG_CAR_CQOS)
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
	div	%ecx				/* result is in eax */
	mov	%eax, %ecx			/* save to ecx */
	mov	$1, %ebx
	shl	%cl, %ebx
	sub	$1, %ebx		/* resulting mask is is in ebx */

	/* Set this mask for initial cache fill */
	mov	$MSR_L2_QOS_MASK(0), %ecx
	rdmsr
	mov	%bl, %al
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
	mov	%bl, %al
	xor	$~0, %al			/* invert 8 bits */
	wrmsr
#endif
	post_code(0x26)

	/* Clear the cache memory region. This will also fill up the cache */
	mov	$CONFIG_DCACHE_RAM_BASE, %edi
	mov	$(CONFIG_DCACHE_RAM_SIZE >> 2), %ecx
	xor	%eax, %eax
	rep	stos %eax, %es:(%edi)

	post_code(0x27)

#if IS_ENABLED(CONFIG_CAR_NEM)
	/* Disable cache eviction (run stage) */
	mov	$MSR_EVICT_CTL, %ecx
	rdmsr
	or	$0x2, %eax
	wrmsr
#else
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
#endif

	post_code(0x28)

car_init_done:

	/* Setup bootblock stack */
	mov	$_car_stack_end, %esp

before_carstage:
	post_code(0x2b)

	/* Restore the timestamp from bootblock_crt0.S (mm2:mm1) */
	movd	%mm2, %eax
	push	%eax
	movd	%mm1, %eax
	push	%eax

	/* We can call into C functions now */
	call bootblock_c_entry

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
