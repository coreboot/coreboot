/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005-2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Carl-Daniel Hailfinger
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

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/mtrr.h>

#define CacheSize		CONFIG_DCACHE_RAM_SIZE
#define CacheBase		(0xd0000 - CacheSize)

/* For CAR with Fam10h. */
#define CacheSizeAPStack	0x400 /* 1K */

#define MSR_MCFG_BASE		0xC0010058
#define MSR_FAM10		0xC001102A

#define jmp_if_k8(x)		comisd	%xmm2, %xmm1; jb x

#define CPUID_MASK		0x0ff00f00
#define CPUID_VAL_FAM10_ROTATED	0x0f000010

/*
 * XMM map:
 *   xmm1: CPU family
 *   xmm2: Fam10h comparison value
 *   xmm3: Backup EBX
 */

	/* Save the BIST result. */
	movl	%eax, %ebp

	/*
	 * For normal part %ebx already contain cpu_init_detected
	 * from fallback call.
	 */

cache_as_ram_setup:
	post_code(0xa0)

	/* Enable SSE. */
	movl	%cr4, %eax
	orl	$(3 << 9), %eax
	movl	%eax, %cr4

	/* Figure out the CPU family. */
	cvtsi2sd %ebx, %xmm3
	movl	$0x01, %eax
	cpuid
	/* Base family is bits 8..11, extended family is bits 20..27. */
	andl	$CPUID_MASK, %eax
	/* Reorder bits for easier comparison by value. */
	roll	$0x10, %eax
	cvtsi2sd %eax, %xmm1
	movl	$CPUID_VAL_FAM10_ROTATED, %eax
	cvtsi2sd %eax, %xmm2
	cvtsd2si %xmm3, %ebx

	/* Check if cpu_init_detected. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$MTRRdefTypeEn, %eax
	movl	%eax, %ebx	/* We store the status. */

	jmp_if_k8(CAR_FAM10_out_post_errata)

	/*
	 * For GH, CAR need to set DRAM Base/Limit registers to direct that
	 * to node0.
	 * Only BSP needed, for other nodes set during HT/memory init.
	 * So we need to check if it is BSP.
	 */
	movl	$0x1b, %ecx
	rdmsr
	bt	$8, %eax	/* BSP */
	jnc	CAR_FAM10_out

	/* Enable RT tables on BSP. */
	movl	$0x8000c06c, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	addw	$4, %dx
	inl	%dx, %eax
	btr	$0, %eax
	outl	%eax, %dx

	/* Setup temporary DRAM map: [0,16M) bit 0-23. */
	movl	$0x8000c144, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	addw	$4, %dx
	movl	$0, %eax
	outl	%eax, %dx

	movl	$0x8000c140, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	addw	$4, %dx
	movl	$3, %eax
	outl	%eax, %dx

CAR_FAM10_out:

	/*
	 * Errata 193: Disable clean copybacks to L3 cache to allow cached ROM.
	 * Re-enable it in after RAM is initialized and before CAR is disabled.
	 */
	movl	$MSR_FAM10, %ecx
	rdmsr
	bts	$15, %eax
	wrmsr

	/* Erratum 343, RevGuide for Fam10h, Pub#41322 Rev. 3.33 */
	movl	$MSR_FAM10, %ecx
	rdmsr
	bts	$35-32, %edx	/* Set bit 35 in EDX:EAX (bit 3 in EDX). */
	wrmsr

#if CONFIG_MMCONF_SUPPORT
   #if (CONFIG_MMCONF_BASE_ADDRESS > 0xFFFFFFFF)
   #error "MMCONF_BASE_ADDRESS too big"
   #elif (CONFIG_MMCONF_BASE_ADDRESS & 0xFFFFF)
   #error "MMCONF_BASE_ADDRESS not 1MB aligned"
   #endif
	movl	$0, %edx
	movl	$((CONFIG_MMCONF_BASE_ADDRESS) | (1 << 0)), %eax
   #if (CONFIG_MMCONF_BUS_NUMBER == 1)
   #elif (CONFIG_MMCONF_BUS_NUMBER == 2)
	orl	$(1 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 4)
	orl	$(2 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 8)
	orl	$(3 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 16)
	orl	$(4 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 32)
	orl	$(5 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 64)
	orl	$(6 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 128)
	orl	$(7 << 2), %eax
   #elif (CONFIG_MMCONF_BUS_NUMBER == 256)
	orl	$(8 << 2), %eax
   #else
	#error "bad MMCONF_BUS_NUMBER value"
   #endif
	movl	$(0xc0010058), %ecx
	wrmsr
#endif

CAR_FAM10_out_post_errata:

	/* Set MtrrFixDramModEn for clear fixed MTRR. */
enable_fixed_mtrr_dram_modify:
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	andl	$(~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrVarDramEn)), %eax
	orl	$SYSCFG_MSR_MtrrFixDramModEn, %eax
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
clear_fixed_var_mtrr_out:

/*
 * 0x06 is the WB IO type for a given 4k segment.
 * 0x1e is the MEM IO type for a given 4k segment (K10 and above).
 * segs is the number of 4k segments in the area of the particular
 *      register we want to use for CAR.
 * reg is the register where the IO type should be stored.
 */
.macro extractmask segs, reg
.if \segs <= 0
	/*
	 * The xorl here is superfluous because at the point of first execution
	 * of this macro, %eax and %edx are cleared. Later invocations of this
	 * macro will have a monotonically increasing segs parameter.
	 */
	xorl	\reg, \reg
.else
	jmp_if_k8(1f)

.if \segs == 1
	movl	$0x1e000000, \reg /* WB MEM type */
.elseif \segs == 2
	movl	$0x1e1e0000, \reg /* WB MEM type */
.elseif \segs == 3
	movl	$0x1e1e1e00, \reg /* WB MEM type */
.elseif \segs >= 4
	movl	$0x1e1e1e1e, \reg /* WB MEM type */
.endif
	jmp 2f
1:
.if \segs == 1
	movl	$0x06000000, \reg /* WB IO type */
.elseif \segs == 2
	movl	$0x06060000, \reg /* WB IO type */
.elseif \segs == 3
	movl	$0x06060600, \reg /* WB IO type */
.elseif \segs >= 4
	movl	$0x06060606, \reg /* WB IO type */
.endif
2:
.endif /* if \segs <= 0 */
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

	/* Enable memory access for first MBs using top_mem. */
	movl	$TOP_MEM, %ecx
	xorl	%edx, %edx
	movl	$(((CONFIG_RAMTOP) + TOP_MEM_MASK) & ~TOP_MEM_MASK) , %eax
	wrmsr

#if CONFIG_XIP_ROM_SIZE

	/* Enable write base caching so we can do execute in place (XIP)
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
	movl	$0xff, %edx /* (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1 for K8 (CONFIG_CPU_ADDR_BITS = 40) */
	jmp_if_k8(wbcache_post_fam10_setup)
	movl	$0xffff, %edx /* (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1 for FAM10 (CONFIG_CPU_ADDR_BITS = 48) */
wbcache_post_fam10_setup:
	movl	$(~(CONFIG_XIP_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	wrmsr
#endif /* CONFIG_XIP_ROM_SIZE */

	/* Set the default memory type and enable fixed and variable MTRRs. */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	movl	$(MTRRdefTypeEn | MTRRdefTypeFixEn), %eax
	wrmsr

	/* Enable the MTRRs and IORRs in SYSCFG. */
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	orl	$(SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn), %eax
	wrmsr

	post_code(0xa1)

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	jmp_if_k8(fam10_end_part1)

	/* So we need to check if it is BSP. */
	movl	$0x1b, %ecx
	rdmsr
	bt	$8, %eax	/* BSP */
	jnc	CAR_FAM10_ap
fam10_end_part1:

	post_code(0xa2)

	/* Read the range with lodsl. */
	cld
	movl	$CacheBase, %esi
	movl	$(CacheSize >> 2), %ecx
	rep	lodsl

	/* Clear the range. */
	movl	$CacheBase, %edi
	movl	$(CacheSize >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	/* Set up the stack pointer. */
	movl	$(CacheBase + CacheSize), %eax
	movl	%eax, %esp

	post_code(0xa3)

	jmp	CAR_FAM10_ap_out
CAR_FAM10_ap:
	/*
	 * Need to set stack pointer for AP.
	 * It will be from:
	 *   CacheBase + CacheSize / 2
	 *   - (NodeID << CoreIDbits + CoreID) * CacheSizeAPStack
	 * So need to get the NodeID and CoreID at first.
	 * If NB_CFG bit 54 is set just use initial APIC ID, otherwise need
	 * to reverse it.
	 */

	/* Store our init detected. */
	movl	%ebx, %esi

	/* Get the coreid bits at first. */
	movl	$0x80000008, %eax
	cpuid
	shrl	$12, %ecx
	andl	$0x0f, %ecx
	movl	%ecx, %edi

	/* Get the initial APIC ID. */
	movl	$1, %eax
	cpuid
	shrl	$24, %ebx

	/* Get the nb cfg bit 54. */
	movl	$0xc001001f, %ecx	/* NB_CFG_MSR */
	rdmsr
	movl	%edi, %ecx		/* CoreID bits */
	bt	$(54 - 32), %edx
	jc	roll_cfg
	rolb	%cl, %bl
roll_cfg:

	/* Calculate stack pointer. */
	movl	$CacheSizeAPStack, %eax
	mull	%ebx
	movl	$(CacheBase + CacheSize / 2), %esp
	subl	%eax, %esp

	/* Retrive init detected. */
	movl	%esi, %ebx

	post_code(0xa4)

CAR_FAM10_ap_out:

	post_code(0xa5)

	/* Disable SSE. */
	movl	%cr4, %eax
	andl	$~(3 << 9), %eax
	movl	%eax, %cr4

	/* Restore the BIST result. */
	movl	%ebp, %eax

	/* We need to set EBP? No need. */
	movl	%esp, %ebp
	pushl	%ebx		/* Init detected. */
	pushl	%eax		/* BIST */
	call	cache_as_ram_main
	/* We will not go back. */

	post_code(0xaf)		/* Should never see this POST code. */

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

	/* Variable IORR MTRR MSRs */
	.long	IORRBase_MSR(0)
	.long	IORRMask_MSR(0)
	.long	IORRBase_MSR(1)
	.long	IORRMask_MSR(1)

	/* Top of memory MTRR MSRs */
	.long	TOP_MEM_MSR
	.long	TOP_MEM2_MSR

	.long	0x000 /* NULL, end of table */

cache_as_ram_setup_out:
