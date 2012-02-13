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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <cpu/x86/stack.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/post_code.h>
#include <cpu/x86/lapic_def.h>

/* Macro to access Local APIC registers at default base. */
#define lapic(x)		$(LAPIC_DEFAULT_BASE | LAPIC_ ## x)

/* Code for CORE (from model_6ex), not for family<6? */
#define CORE_L2CACHE_MSR 1

/* Code for CORE2 (from model_6fx), not built. */
#define CORE2_PREFETCH_DISABLE 0

/* MAXPHYADDR for Atom (model_106cx) is 32. */
#if CONFIG_CPU_INTEL_MODEL_106CX
#define CPU_MAXPHYADDR 32
#else
#define CPU_MAXPHYADDR 36
#endif
#define CPU_PHYSMASK_HI  (1 << (CPU_MAXPHYADDR - 32) - 1)

#define ENABLE_BOOTROM_CACHE	1
#define SPURIOUS_CACHE_CTRL	1

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

	/* Save the BIST result. */
	movl	%eax, %ebp

cache_as_ram:
	post_code(0x20)

	/* Send INIT IPI to all excluding ourself. */
	movl	lapic(ICR), %edi
	movl	$(LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT | LAPIC_DM_INIT), %eax
	movl	%eax, (%edi)

#if CORE2_PREFETCH_DISABLE
	/* Disable prefetchers */
	movl	$0x01a0, %ecx
	rdmsr
	orl	$((1 << 9) | (1 << 19)), %eax
	orl	$((1 << 5) | (1 << 7)), %edx
	wrmsr
#endif

	/* Zero out all fixed range and variable range MTRRs. */
	movl	$mtrr_table, %esi
	movl	$((mtrr_table_end - mtrr_table) / 2), %edi
	xorl	%eax, %eax
	xorl	%edx, %edx
clear_mtrrs:
	movw	(%esi), %bx
	movzx	%bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	clear_mtrrs

	/* Configure the default memory type to uncacheable. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	/* Set Cache-as-RAM base address. */
	movl	$(MTRRphysBase_MSR(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	/* Set Cache-as-RAM mask. */
	movl	$(MTRRphysMask_MSR(0)), %ecx
	movl	$(~(CACHE_AS_RAM_SIZE - 1) | MTRRphysMaskValid), %eax
	movl	$CPU_PHYSMASK_HI, %edx
	wrmsr

	/* Enable variable MTRRs. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	orl	$MTRRdefTypeEn, %eax
	wrmsr

#if CORE_L2CACHE_MSR
	/* Enable L2 cache. */
	movl	$0x11e, %ecx
	rdmsr
	orl	$(1 << 8), %eax
	wrmsr
#endif

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~((1 << 30) | (1 << 29))), %eax
	invd
	movl	%eax, %cr0
	invd

	/* Clear the cache memory reagion. */
	cld
	xorl	%eax, %eax
	movl	$CACHE_AS_RAM_BASE, %edi
	movl	$(CACHE_AS_RAM_SIZE / 4), %ecx
	rep	stosl

#if SPURIOUS_CACHE_CTRL
	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$(1 << 30), %eax
	movl	%eax, %cr0
#endif

#if CONFIG_XIP_ROM_SIZE
	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRRphysBase_MSR(1), %ecx
	xorl	%edx, %edx
	/*
	 * IMPORTANT: The following calculation _must_ be done at runtime. See
	 * http://www.coreboot.org/pipermail/coreboot/2010-October/060855.html
	 */
	movl	$copy_and_run, %eax
	andl	$(~(CONFIG_XIP_ROM_SIZE - 1)), %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	wrmsr

	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$CPU_PHYSMASK_HI, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	wrmsr
#endif /* CONFIG_XIP_ROM_SIZE */

#if SPURIOUS_CACHE_CTRL
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~((1 << 30) | (1 << 29))), %eax
	movl	%eax, %cr0
#endif

	/* Set up the stack pointer. */
#if CONFIG_USBDEBUG
	/* Leave some space for the struct ehci_debug_info. */
	movl	$(CACHE_AS_RAM_BASE + CACHE_AS_RAM_SIZE - 4 - 128), %esp
#else
	movl	$(CACHE_AS_RAM_BASE + CACHE_AS_RAM_SIZE - 4), %esp
#endif

	/* Restore the BIST result. */
	movl	%ebp, %eax
	movl	%esp, %ebp
	pushl	%eax

	post_code(0x23)

	/* Call mainboard/romstage.c: main(). */
	call	main
	addl	$4, %esp

	post_code(0x2f)

	post_code(0x30)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$(1 << 30), %eax
	movl	%eax, %cr0

	post_code(0x31)

	/* Disable MTRR. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$(~MTRRdefTypeEn), %eax
	wrmsr

	post_code(0x31)

	invd

	post_code(0x33)

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$~((1 << 30) | (1 << 29)), %eax
	movl	%eax, %cr0

	post_code(0x36)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$(1 << 30), %eax
	movl	%eax, %cr0

	post_code(0x38)

#if CONFIG_RAMTOP
	/* Enable Write Back and Speculative Reads for low RAM. */
	movl	$MTRRphysBase_MSR(0), %ecx
	movl	$(0x00000000 | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr
	movl	$MTRRphysMask_MSR(0), %ecx
	movl	$(~(CONFIG_RAMTOP - 1) | MTRRphysMaskValid), %eax
	movl	$CPU_PHYSMASK_HI, %edx
	wrmsr
#endif

#if ENABLE_BOOTROM_CACHE
	/* Enable caching and Speculative Reads for the last 4MB. */
	movl	$MTRRphysBase_MSR(1), %ecx
	movl	$(0xffc00000 | MTRR_TYPE_WRPROT), %eax
	xorl	%edx, %edx
	wrmsr
	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$(~(4 * 1024 * 1024 - 1) | MTRRphysMaskValid), %eax
	movl	$CPU_PHYSMASK_HI, %edx
	wrmsr
#endif

	post_code(0x39)

	/* And enable cache again after setting MTRRs. */
	movl	%cr0, %eax
	andl	$~((1 << 30) | (1 << 29)), %eax
	movl	%eax, %cr0

	post_code(0x3a)

	/* Enable MTRR. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	orl	$MTRRdefTypeEn, %eax
	wrmsr

	post_code(0x3b)

#if CORE2_PREFETCH_DISABLE
	/* Enable prefetchers */
	movl	$0x01a0, %ecx
	rdmsr
	andl	$~((1 << 9) | (1 << 19)), %eax
	andl	$~((1 << 5) | (1 << 7)), %edx
	wrmsr
#endif

	/* Invalidate the cache again. */
	invd

	post_code(0x3c)

	/* Clear boot_complete flag. */
	xorl	%ebp, %ebp
__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	movl	%ebp, %esi

	movl	$ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	pushl	%esi
	call	copy_and_run

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
	/* Variable MTRRs */
	.word 0x200, 0x201, 0x202, 0x203
	.word 0x204, 0x205, 0x206, 0x207
	.word 0x208, 0x209, 0x20A, 0x20B
	.word 0x20C, 0x20D, 0x20E, 0x20F
mtrr_table_end:

