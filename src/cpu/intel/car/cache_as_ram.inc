/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000, 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Eswar Nallusamy, LANL
 * Copyright (C) 2005 Tyan (written by Yinghai Lu for Tyan)
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cpu/x86/stack.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/post_code.h>

#define CacheSize		CONFIG_DCACHE_RAM_SIZE
#define CacheBase		(0xd0000 - CacheSize)

	/* Save the BIST result. */
	movl	%eax, %ebp

CacheAsRam:
	/* Check whether the processor has HT capability. */
	movl	$01, %eax
	cpuid
	btl	$28, %edx
	jnc	NotHtProcessor
	bswapl	%ebx
	cmpb	$01, %bh
	jbe	NotHtProcessor

	/*
	 * It is a HT processor. Send SIPI to the other logical processor
	 * within this processor so that the CAR related common system
	 * registers are programmed accordingly.
	 */

	/*
	 * Use some register that is common to both logical processors
	 * as semaphore. Refer Appendix B, Vol.3.
	 */
	xorl	%eax, %eax
	xorl	%edx, %edx
	movl	$MTRRfix64K_00000_MSR, %ecx
	wrmsr

	/*
	 * Figure out the logical AP's APIC ID; the following logic will
	 * work only for processors with 2 threads.
	 * Refer to Vol 3. Table 7-1 for details about this logic.
	 */
	movl	$0xFEE00020, %esi
	movl	(%esi), %ebx
	andl	$0xFF000000, %ebx
	bswapl	%ebx
	btl	$0, %ebx
	jnc	LogicalAP0
	andb	$0xFE, %bl
	jmp	Send_SIPI
LogicalAP0:
	orb	$0x01, %bl
Send_SIPI:
	bswapl	%ebx	/* EBX - logical AP's APIC ID. */

	/*
	 * Fill up the IPI command registers in the Local APIC mapped to
	 * default address and issue SIPI to the other logical processor
	 * within this processor die.
	 */
Retry_SIPI:
	movl	%ebx, %eax
	movl	$0xFEE00310, %esi
	movl	%eax, (%esi)

	/* SIPI vector - F900:0000 */
	movl	$0x000006F9, %eax
	movl	$0xFEE00300, %esi
	movl	%eax, (%esi)

	movl	$0x30, %ecx
SIPI_Delay:
	pause
	decl	%ecx
	jnz	SIPI_Delay

	movl	(%esi), %eax
	andl	$0x00001000, %eax
	jnz	Retry_SIPI

	/* Wait for the Logical AP to complete initialization. */
LogicalAP_SIPINotdone:
	movl	$MTRRfix64K_00000_MSR, %ecx
	rdmsr
	orl	%eax, %eax
	jz	LogicalAP_SIPINotdone

NotHtProcessor:
	/* Set the default memory type and enable fixed and variable MTRRs. */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn | MTRRdefTypeFixEn), %eax
	wrmsr

	/* Clear all MTRRs. */
	xorl	%edx, %edx
	movl	$all_mtrr_msrs, %esi

clear_fixed_var_mtrr:
	lodsl	(%esi), %eax
	testl	%eax, %eax
	jz	clear_fixed_var_mtrr_out

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp	clear_fixed_var_mtrr

all_mtrr_msrs:
	/* fixed MTRR MSRs */
	.long	MTRRfix64K_00000_MSR
	.long	MTRRfix16K_80000_MSR
	.long	MTRRfix16K_A0000_MSR
	.long	MTRRfix4K_C0000_MSR
	.long	MTRRfix4K_C8000_MSR
	.long	MTRRfix4K_D0000_MSR
	.long	MTRRfix4K_D8000_MSR
	.long	MTRRfix4K_E0000_MSR
	.long	MTRRfix4K_E8000_MSR
	.long	MTRRfix4K_F0000_MSR
	.long	MTRRfix4K_F8000_MSR

	/* var MTRR MSRs */
	.long	MTRRphysBase_MSR(0)
	.long	MTRRphysMask_MSR(0)
	.long	MTRRphysBase_MSR(1)
	.long	MTRRphysMask_MSR(1)
	.long	MTRRphysBase_MSR(2)
	.long	MTRRphysMask_MSR(2)
	.long	MTRRphysBase_MSR(3)
	.long	MTRRphysMask_MSR(3)
	.long	MTRRphysBase_MSR(4)
	.long	MTRRphysMask_MSR(4)
	.long	MTRRphysBase_MSR(5)
	.long	MTRRphysMask_MSR(5)
	.long	MTRRphysBase_MSR(6)
	.long	MTRRphysMask_MSR(6)
	.long	MTRRphysBase_MSR(7)
	.long	MTRRphysMask_MSR(7)

	.long	0x000 /* NULL, end of table */

clear_fixed_var_mtrr_out:

/*
 * 0x06 is the WB IO type for a given 4k segment.
 * segs is the number of 4k segments in the area of the particular
 *      register we want to use for CAR.
 * reg  is the register where the IO type should be stored.
 */
.macro extractmask segs, reg
.if \segs <= 0
	/*
	 * The xorl here is superfluous because at the point of first execution
	 * of this macro, %eax and %edx are cleared. Later invocations of this
	 * macro will have a monotonically increasing segs parameter.
	 */
	xorl \reg, \reg
.elseif \segs == 1
	movl	$0x06000000, \reg /* WB IO type */
.elseif \segs == 2
	movl	$0x06060000, \reg /* WB IO type */
.elseif \segs == 3
	movl	$0x06060600, \reg /* WB IO type */
.elseif \segs >= 4
	movl	$0x06060606, \reg /* WB IO type */
.endif
.endm

/*
 * carsize is the cache size in bytes we want to use for CAR.
 * windowoffset is the 32k-aligned window into CAR size.
 */
.macro simplemask carsize, windowoffset
	.set gas_bug_workaround,(((\carsize - \windowoffset) / 0x1000) - 4)
	extractmask gas_bug_workaround, %eax
	.set gas_bug_workaround,(((\carsize - \windowoffset) / 0x1000))
	extractmask gas_bug_workaround, %edx
	/*
	 * Without the gas bug workaround, the entire macro would consist
	 * only of the two lines below:
	 *   extractmask (((\carsize - \windowoffset) / 0x1000) - 4), %eax
	 *   extractmask (((\carsize - \windowoffset) / 0x1000)), %edx
	 */
.endm

#if CacheSize > 0x10000
#error Invalid CAR size, must be at most 64k.
#endif
#if CacheSize < 0x1000
#error Invalid CAR size, must be at least 4k. This is a processor limitation.
#endif
#if (CacheSize & (0x1000 - 1))
#error Invalid CAR size, is not a multiple of 4k. This is a processor limitation.
#endif

#if CacheSize > 0x8000
	/* Enable caching for 32K-64K using fixed MTRR. */
	movl	$MTRRfix4K_C0000_MSR, %ecx
	simplemask CacheSize, 0x8000
	wrmsr
#endif

	/* Enable caching for 0-32K using fixed MTRR. */
	movl	$MTRRfix4K_C8000_MSR, %ecx
	simplemask CacheSize, 0
	wrmsr

#if CONFIG_XIP_ROM_SIZE

	/*
	 * Enable write base caching so we can do execute in place (XIP)
	 * on the flash ROM.
	 */
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
	movl	$0x0000000f, %edx
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	wrmsr
#endif /* CONFIG_XIP_ROM_SIZE */

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	/* Read the range with lodsl. */
	movl	$CacheBase, %esi
	cld
	movl	$(CacheSize >> 2), %ecx
	rep	lodsl

	/* Clear the range. */
	movl	$CacheBase, %edi
	movl	$(CacheSize >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

#if 0
	/* Check the cache as ram. */
	movl	$CacheBase, %esi
	movl	$(CacheSize >> 2), %ecx
.xin1:
	movl	%esi, %eax
	movl	%eax, (%esi)
	decl	%ecx
	je	.xout1
	add	$4, %esi
	jmp	.xin1
.xout1:

	movl	$CacheBase, %esi
	// movl	$(CacheSize >> 2), %ecx
	movl	$4, %ecx
.xin1x:
	movl	%esi, %eax

	movl	$0x4000, %edx
	movb	%ah, %al
.testx1:
	outb	%al, $0x80
	decl	%edx
	jnz	.testx1

	movl	(%esi), %eax
	cmpb	0xff, %al
	je	.xin2	/* Don't show. */

	movl	$0x4000, %edx
.testx2:
	outb	%al, $0x80
	decl	%edx
	jnz	.testx2

.xin2:
	decl	%ecx
	je	.xout1x
	add	$4, %esi
	jmp	.xin1x
.xout1x:
#endif

	movl	$(CacheBase + CacheSize - 4), %eax
	movl	%eax, %esp
lout:
	/* Restore the BIST result. */
	movl	%ebp, %eax

	/* We need to set EBP? No need. */
	movl	%esp, %ebp
	pushl	%eax  /* BIST */
	call	main

	/* We don't need CAR from now on. */

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/* Clear sth. */
	movl	$MTRRfix4K_C8000_MSR, %ecx
	xorl	%edx, %edx
	xorl	%eax, %eax
	wrmsr

#if CONFIG_DCACHE_RAM_SIZE > 0x8000
	movl	$MTRRfix4K_C0000_MSR, %ecx
	wrmsr
#endif

	/*
	 * Set the default memory type and disable fixed
	 * and enable variable MTRRs.
	 */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$MTRRdefTypeEn, %eax /* Enable variable and disable fixed MTRRs. */
	wrmsr

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	movl	$ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	call	copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

