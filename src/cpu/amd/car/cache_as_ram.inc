/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2005-2007 Advanced Micro Devices, Inc.
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

#define CacheSize DCACHE_RAM_SIZE
#define CacheBase (0xd0000 - CacheSize)

/* leave some space for global variable to pass to RAM stage */
#define GlobalVarSize DCACHE_RAM_GLOBAL_VAR_SIZE

#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>

	/* Save the BIST result */
	movl	%eax, %ebp

	/* for normal part %ebx already contain cpu_init_detected from fallback call */

cache_as_ram_setup:

	/* hope we can skip the double set for normal part */
#if ((HAVE_FAILOVER_BOOT == 1) && (USE_FAILOVER_IMAGE == 1)) || ((HAVE_FAILOVER_BOOT == 0) && (USE_FALLBACK_IMAGE == 1))

	/* check if cpu_init_detected */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$(1 << 11), %eax
	movl	%eax, %ebx	/* We store the status */

	/* Set MtrrFixDramModEn for clear fixed mtrr */
enable_fixed_mtrr_dram_modify:
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	andl	$(~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrVarDramEn)), %eax
	orl	$SYSCFG_MSR_MtrrFixDramModEn, %eax
	wrmsr

	/* Clear all MTRRs */
	xorl	%edx, %edx
	movl	$fixed_mtrr_msr, %esi

clear_fixed_var_mtrr:
	lodsl	(%esi), %eax
	testl	%eax, %eax
	jz		clear_fixed_var_mtrr_out

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp		clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:

#if CacheSize == 0x10000
	/* enable caching for 64K using fixed mtrr */
	movl	$0x268, %ecx		/* fix4k_c0000 */
	movl	$0x06060606, %eax	/* WB IO type */
	movl	%eax, %edx
	wrmsr
	movl	$0x269, %ecx
	wrmsr
#endif

#if CacheSize == 0xc000
	/* enable caching for 16K using fixed mtrr */
	movl	$0x268, %ecx		/* fix4k_c4000 */
	movl	$0x06060606, %edx	/* WB IO type */
	xorl	%eax, %eax
	wrmsr
	/* enable caching for 32K using fixed mtrr */
	movl	$0x269, %ecx		/* fix4k_c8000 */
	movl	$0x06060606, %eax	/* WB IO type */
	movl	%eax, %edx
	wrmsr
#endif


#if CacheSize == 0x8000
	/* enable caching for 32K using fixed mtrr */
	movl	$0x269, %ecx		/* fix4k_c8000 */
	movl	$0x06060606, %eax	/* WB IO type */
	movl	%eax, %edx
	wrmsr
#endif

#if CacheSize < 0x8000
	/* enable caching for 16K/8K/4K using fixed mtrr */
	movl	$0x269, %ecx  /* fix4k_cc000*/
#if CacheSize == 0x4000
	movl	$0x06060606, %edx /* WB IO type */
#endif
#if CacheSize == 0x2000
	movl	$0x06060000, %edx /* WB IO type */
#endif
#if CacheSize == 0x1000
	movl	$0x06000000, %edx /* WB IO type */
#endif
	xorl	%eax, %eax
	wrmsr
#endif

	/* enable memory access for first MBs using top_mem */
	movl	$TOP_MEM, %ecx
	xorl	%edx, %edx
	movl	$(((CONFIG_LB_MEM_TOPK << 10) + TOP_MEM_MASK) & ~TOP_MEM_MASK) , %eax
	wrmsr
#endif /*  USE_FAILOVER_IMAGE == 1*/


#if ((HAVE_FAILOVER_BOOT == 1) && (USE_FAILOVER_IMAGE == 0)) || ((HAVE_FAILOVER_BOOT == 0) && (USE_FALLBACK_IMAGE == 0))
	/* disable cache */
	movl	%cr0, %eax
	orl		$(1 << 30),%eax
	movl	%eax, %cr0

#endif

#if defined(XIP_ROM_SIZE) && defined(XIP_ROM_BASE)
	/* enable write base caching so we can do execute in place
	 * on the flash rom.
	 */
	movl	$0x202, %ecx
	xorl	%edx, %edx
	movl	$(XIP_ROM_BASE | MTRR_TYPE_WRBACK), %eax
	wrmsr

	movl	$0x203, %ecx
	movl	$((1 << (CPU_ADDR_BITS - 32)) - 1), %edx  /* AMD 40 bit for K8, 48 bit for GH */
	movl	$(~(XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

#if ((HAVE_FAILOVER_BOOT == 1) && (USE_FAILOVER_IMAGE == 1)) || ((HAVE_FAILOVER_BOOT == 0) && (USE_FALLBACK_IMAGE == 1))
	/* Set the default memory type and enable fixed and variable MTRRs */
	movl	$MTRRdefType_MSR, %ecx
	xorl	%edx, %edx
	/* Enable Variable and Fixed MTRRs */
	movl	$0x00000c00, %eax
	wrmsr

	/* Enable the MTRRs and IORRs in SYSCFG */
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	orl		$(SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn), %eax
	wrmsr
#endif

	/* enable cache */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

#if ((HAVE_FAILOVER_BOOT == 1) && (USE_FAILOVER_IMAGE == 1)) || ((HAVE_FAILOVER_BOOT == 0) && (USE_FALLBACK_IMAGE == 1))

		/* Read the range with lodsl*/
	cld
	movl	$CacheBase, %esi
	movl	$(CacheSize >> 2), %ecx
	rep		lodsl
	/* Clear the range */
	movl	$CacheBase, %edi
	movl	$(CacheSize >> 2), %ecx
	xorl	%eax, %eax
	rep		stosl

#endif /*USE_FAILOVER_IMAGE == 1*/

	/* set up the stack pointer */
	movl	$(CacheBase + CacheSize - GlobalVarSize), %eax
	movl	%eax, %esp

	/* Restore the BIST result */
	movl	%ebp, %eax

	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl	%ebx  /* init detected */
	pushl	%eax  /* bist */
	call	cache_as_ram_main
	/* We will not go back */

fixed_mtrr_msr:
	.long	0x250, 0x258, 0x259
	.long	0x268, 0x269, 0x26A
	.long	0x26B, 0x26C, 0x26D
	.long	0x26E, 0x26F
var_mtrr_msr:
	.long	0x200, 0x201, 0x202, 0x203
	.long	0x204, 0x205, 0x206, 0x207
	.long	0x208, 0x209, 0x20A, 0x20B
	.long	0x20C, 0x20D, 0x20E, 0x20F
var_iorr_msr:
	.long	0xC0010016, 0xC0010017, 0xC0010018, 0xC0010019
mem_top:
	.long	0xC001001A, 0xC001001D
	.long	0x000 /* NULL, end of table */

cache_as_ram_setup_out:
