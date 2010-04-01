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

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>

	/* Save the BIST result */
	movl    %eax, %ebp

cache_as_ram:
	post_code(0x20)

	/* Send INIT IPI to all excluding ourself */
        movl	$0x000C4500, %eax
        movl	$0xFEE00300, %esi
        movl	%eax, (%esi)

	post_code(0x21)

	/* Zero out all Fixed Range and Variable Range MTRRs */
	movl	$mtrr_table, %esi
	movl	$( (mtrr_table_end - mtrr_table) / 2), %edi
	xorl    %eax, %eax
	xorl    %edx, %edx
clear_mtrrs:
	movw    (%esi), %bx
	movzx   %bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	clear_mtrrs
	post_code(0x22)

	/* Configure the default memory type to uncacheable */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(0x23)
	/* Set cache as ram base address */
	movl	$(MTRRphysBase_MSR(0)), %ecx
	movl	$(CACHE_AS_RAM_BASE | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x24)
	/* Set cache as ram mask */
	movl	$(MTRRphysMask_MSR(0)), %ecx
	movl	$(~((CACHE_AS_RAM_SIZE-1)) | (1 << 11)), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(0x25)
	/* Enable MTRR */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	orl	$(1 << 11), %eax
	wrmsr

	post_code(0x26)
	/* Enable L2 Cache */
	movl	$0x11e, %ecx
	rdmsr
	orl	$(1 << 8), %eax
	wrmsr

	post_code(0x27)
	/* CR0.CD = 0, CR0.NW = 0 */
        movl	%cr0, %eax
	andl	$( ~( (1 << 30) | (1 << 29) ) ), %eax
	invd
	movl	%eax, %cr0

	post_code(0x28)
	/* Clear the cache memory reagion */
	movl	$CACHE_AS_RAM_BASE, %esi
	movl	%esi, %edi
	movl	$(CACHE_AS_RAM_SIZE / 4), %ecx
	//movl	$0x23322332, %eax
	xorl	%eax, %eax
	rep	stosl

	post_code(0x29)
	/* Enable Cache As RAM mode by disabling cache */
	movl	%cr0, %eax
	orl	$(1 << 30), %eax
	movl	%eax, %cr0

#if defined(CONFIG_XIP_ROM_SIZE) && defined(CONFIG_XIP_ROM_BASE)
	/* Enable cache for our code in Flash because we do XIP here */
        movl    $MTRRphysBase_MSR(1), %ecx
	xorl    %edx, %edx
#if defined(CONFIG_TINY_BOOTBLOCK) && CONFIG_TINY_BOOTBLOCK
#define REAL_XIP_ROM_BASE AUTO_XIP_ROM_BASE
#else
#define REAL_XIP_ROM_BASE CONFIG_XIP_ROM_BASE
#endif
        movl    $REAL_XIP_ROM_BASE, %eax
        orl     $MTRR_TYPE_WRBACK, %eax
        wrmsr

        movl    $MTRRphysMask_MSR(1), %ecx
	xorl	%edx, %edx
        movl    $(~(CONFIG_XIP_ROM_SIZE - 1) | 0x800), %eax
        wrmsr
#endif /* CONFIG_XIP_ROM_SIZE && CONFIG_XIP_ROM_BASE */

	post_code(0x2a)
        /* enable cache */
        movl	%cr0, %eax
	andl	$( ~( (1 << 30) | (1 << 29) ) ), %eax
	movl	%eax, %cr0

	/* Set up stack pointer */
#if defined(CONFIG_USBDEBUG_DIRECT) && (CONFIG_USBDEBUG_DIRECT == 1)
	/* leave some space for the struct ehci_debug_info */
	movl	$(CACHE_AS_RAM_BASE + CACHE_AS_RAM_SIZE - 4 - 128), %eax
#else
	movl	$(CACHE_AS_RAM_BASE + CACHE_AS_RAM_SIZE - 4), %eax
#endif
	movl	%eax, %esp

	/* Restore the BIST result */
	movl	%ebp, %eax
	movl	%esp, %ebp
	pushl	%eax

	post_code(0x23)

	call	stage1_main

	post_code(0x2f)
error:
	hlt
	jmp	error

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

