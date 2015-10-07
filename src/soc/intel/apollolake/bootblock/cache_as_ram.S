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
 */

#include <device/pci_def.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>

#define EVICT_CTL_MSR		0x2e0

.global bootblock_pre_c_entry
bootblock_pre_c_entry:
	/*
	 * eax:  BIST value
	 */
	movd	%eax, %mm2

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
	and	$MTRR_DEF_TYPE_MASK, %eax
	wrmsr

	post_code(0x24)

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
	mov	$EVICT_CTL_MSR, %ecx
	rdmsr
	or	$0x1, %eax
	wrmsr

	post_code(0x26)

	/* Clear the cache memory region. This will also fill up the cache */
	mov	$CONFIG_DCACHE_RAM_BASE, %edi
	mov	$(CONFIG_DCACHE_RAM_SIZE >> 2), %ecx
	xor	%eax, %eax
	rep	stos %eax, %es:(%edi)

	post_code(0x27)

	/* Disable cache eviction (run stage) */
	mov	$EVICT_CTL_MSR, %ecx
	rdmsr
	or	$0x2, %eax
	wrmsr

	post_code(0x28)

car_init_done:

	/* Setup bootblock stack */
	mov	$_car_stack_end, %esp

before_carstage:
	post_code(0x2b)

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
