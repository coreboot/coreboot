/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Tyan (written by Yinghai Lu for Tyan)
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2012 Kyösti Mälkki <kyosti.malkki@gmail.com>
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
#include <cpu/x86/lapic_def.h>

/* Macro to access Local APIC registers at default base. */
#define LAPIC(x)		$(LAPIC_DEFAULT_BASE | LAPIC_ ## x)
#define START_IPI_VECTOR	((CONFIG_AP_SIPI_VECTOR >> 12) & 0xff)

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

.code32
_cache_as_ram_setup:

	/* Save the BIST result. */
	movl	%eax, %ebp

cache_as_ram:
	post_code(0x20)

	movl	$LAPIC_BASE_MSR, %ecx
	rdmsr
	andl	$LAPIC_BASE_MSR_BOOTSTRAP_PROCESSOR, %eax
	jz	ap_init

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
	post_code(0x21)

	/* Configure the default memory type to uncacheable. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(0x22)

	/* Determine CPU_ADDR_BITS and load PHYSMASK high
	 * word to %edx.
	 */
	movl	$0x80000000, %eax
	cpuid
	cmpl	$0x80000008, %eax
	jc	addrsize_no_MSR
	movl	$0x80000008, %eax
	cpuid
	movb	%al, %cl
	sub	$32, %cl
	movl	$1, %edx
	shl	%cl, %edx
	subl	$1, %edx
	jmp	addrsize_set_high
addrsize_no_MSR:
	movl	$1, %eax
	cpuid
	andl	$(1 << 6 | 1 << 17), %edx	/* PAE or PSE36 */
	jz	addrsize_set_high
	movl	$0x0f, %edx

	/* Preload high word of address mask (in %edx) for Variable
	 * MTRRs 0 and 1 and enable local APIC at default base.
	 */
addrsize_set_high:
	xorl	%eax, %eax
	movl	$MTRR_PHYS_MASK(0), %ecx
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	wrmsr
	movl	$LAPIC_BASE_MSR, %ecx
	not	%edx
	movl	%edx, %ebx
	rdmsr
	andl	%ebx, %edx
	andl	$(~LAPIC_BASE_MSR_ADDR_MASK), %eax
	orl	$(LAPIC_DEFAULT_BASE | LAPIC_BASE_MSR_ENABLE), %eax
	wrmsr

bsp_init:

	post_code(0x23)

	/* Send INIT IPI to all excluding ourself. */
	movl	LAPIC(ICR), %edi
	movl	$(LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT | LAPIC_DM_INIT), %eax
1:	movl	%eax, (%edi)
	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	movl	(%edi), %ecx
	andl	$LAPIC_ICR_BUSY, %ecx
	jnz	1b

	post_code(0x24)

	movl	$1, %eax
	cpuid
	btl	$28, %edx
	jnc	sipi_complete
	bswapl	%ebx
	movzx	%bh, %edi
	cmpb	$1, %bh
	jbe	sipi_complete	/* only one LAPIC ID in package */

	movl	$0, %eax
	cpuid
	movb	$1, %bl
	cmpl	$4, %eax
	jb	cores_counted
	movl	$4, %eax
	movl	$0, %ecx
	cpuid
	shr	$26, %eax
	movb	%al, %bl
	inc	%bl

cores_counted:
	movl	%edi, %eax
	divb	%bl
	cmpb	$1, %al
	jbe	sipi_complete	/* only LAPIC ID of a core */

	/* For a hyper-threading processor, cache must not be disabled
	 * on an AP on the same physical package with the BSP.
	 */

hyper_threading_cpu:

	/* delay 10 ms */
	movl	$10000, %ecx
1:	inb	$0x80, %al
	dec	%ecx
	jnz	1b

	post_code(0x25)

	/* Send Start IPI to all excluding ourself. */
	movl	LAPIC(ICR), %edi
	movl	$(LAPIC_DEST_ALLBUT | LAPIC_DM_STARTUP | START_IPI_VECTOR), %eax
1:	movl	%eax, (%edi)
	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	movl	(%edi), %ecx
	andl	$LAPIC_ICR_BUSY, %ecx
	jnz	1b

	/* delay 250 us */
	movl	$250, %ecx
1:	inb	$0x80, %al
	dec	%ecx
	jnz	1b

	post_code(0x26)

	/* Wait for sibling CPU to start. */
1:	movl	$(MTRR_PHYS_BASE(0)), %ecx
	rdmsr
	andl	%eax, %eax
	jnz	sipi_complete

	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	jmp	1b


ap_init:
	post_code(0x27)

	/* Do not disable cache (so BSP can enable it). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	post_code(0x28)

	/* MTRR registers are shared between HT siblings. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(1 << 12), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x29)

ap_halt:
	cli
1:	hlt
	jmp	1b



sipi_complete:

	post_code(0x2a)

	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	movl	$(~(CACHE_AS_RAM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	post_code(0x2b)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable L2 cache Write-Back (WBINVD and FLUSH#).
	 *
	 * MSR is set when DisplayFamily_DisplayModel is one of:
	 * 06_0x, 06_17, 06_1C
	 *
	 * Description says this bit enables use of WBINVD and FLUSH#.
	 * Should this be set only after the system bus and/or memory
	 * controller can successfully handle write cycles?
	 */

#define EAX_FAMILY(a)	(a << 8)	/* for family <= 0fH */
#define EAX_MODEL(a)	(((a & 0xf0) << 12) | ((a & 0xf) << 4))

	movl	$1, %eax
	cpuid
	movl	%eax, %ebx
	andl	$EAX_FAMILY(0x0f), %eax
	cmpl	$EAX_FAMILY(0x06), %eax
	jne	no_msr_11e
	movl	%ebx, %eax
	andl	$EAX_MODEL(0xff), %eax
	cmpl	$EAX_MODEL(0x17), %eax
	je	has_msr_11e
	cmpl	$EAX_MODEL(0x1c), %eax
	je	has_msr_11e
	andl	$EAX_MODEL(0xf0), %eax
	cmpl	$EAX_MODEL(0x00), %eax
	jne	no_msr_11e
has_msr_11e:
	movl	$0x11e, %ecx
	rdmsr
	orl	$(1 << 8), %eax
	wrmsr
no_msr_11e:

	post_code(0x2c)

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* Clear the cache memory region. This will also fill up the cache. */
	cld
	xorl	%eax, %eax
	movl	$CACHE_AS_RAM_BASE, %edi
	movl	$(CACHE_AS_RAM_SIZE >> 2), %ecx
	rep	stosl

	post_code(0x2d)
	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/*
	 * An unidentified combination of speculative reads and branch
	 * predictions inside WRPROT-cacheable memory can cause invalidation
	 * of cachelines and loss of stack on models based on NetBurst
	 * microarchitecture. Therefore disable WRPROT region entirely for
	 * all family F models.
	 */
	movl	$1, %eax
	cpuid
	cmp	$0xf, %ah
	je	skip_cache_rom

	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * https://mail.coreboot.org/pipermail/coreboot/2010-October/060922.html
	 */
	movl	$_program, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

skip_cache_rom:

	post_code(0x2e)
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Setup the stack. */
	movl	$(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE), %eax
	movl	%eax, %esp
	/* Align the stack 16 bytes */
	andl	$0xfffffff0, %esp
	/* Account for pushing the BIST result */
	subl	$12, %esp

	/* Restore the BIST result. */
	movl	%ebp, %eax
	movl	%esp, %ebp
	pushl	%eax

before_romstage:
	post_code(0x2f)
	/* Call romstage.c main function. */
	call	romstage_main

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
