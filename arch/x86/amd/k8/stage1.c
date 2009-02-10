/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 Marc Jones <marcj303@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <msr.h>
#include <macros.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include <stage1.h>

/**
 * Set the MTRRs of the current core for initial ram access.
 * Be warned, this function is used by the BSP and all AP cores. TOM and TOM2
 * of the calling core must be setup. This function may be called several times
 * depending on MEM_TRAIN_SEQ.
 */
void set_mtrr_ram_access(void)
{
	struct msr msr;

	disable_cache();

	/* 0 - 640KB */
	stage1_set_fix_mtrr(MTRRfix64K_00000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix16K_80000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);

	/* 0xA0000 - 0xC0000 (UC to video card) */
	stage1_set_fix_mtrr(MTRRfix16K_A0000_MSR, MTRR_TYPE_UNCACHEABLE);

	/* 0xC0000 - 1MB */
	stage1_set_fix_mtrr(MTRRfix4K_C0000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_C8000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_D0000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_D8000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_E0000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_E8000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_F0000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);
	stage1_set_fix_mtrr(MTRRfix4K_F8000_MSR,
						 MTRR_READ_MEM | MTRR_WRITE_MEM | MTRR_TYPE_WRBACK);

	/* 1MB - TOM */
	msr = rdmsr(TOP_MEM);
	stage1_set_var_mtrr_x(0, 0x00100000, 0x0, msr.lo, msr.hi, MTRR_TYPE_WRBACK);

	/* System ROM (Assume 1MB) */
	stage1_set_var_mtrr(1, 0xFFFFFFFF - (u32)((CONFIG_COREBOOT_ROMSIZE_KB << 10) - 1),
						CONFIG_COREBOOT_ROMSIZE_KB << 10, MTRR_TYPE_WRTHROUGH);

	/* 4GB - TOM2 */
	msr = rdmsr(SYSCFG_MSR);
	if (msr.lo & SYSCFG_MSR_TOM2En) {
		msr = rdmsr(TOP_MEM2);
		stage1_set_var_mtrr_x(2, 0x0, 0x00000001, msr.lo, msr.hi,
							  MTRR_TYPE_WRBACK);
	}

	/* Enable Fixed and Variable MTRRs MSRs*/
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= (SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn);
	wrmsr(SYSCFG_MSR, msr);

	/* Enable Fixed and Variable MTRRs */
	msr = rdmsr(MTRRdefType_MSR);
	msr.lo |= 0xC00;
	wrmsr(MTRRdefType_MSR, msr);

	enable_cache();
}


/* This function MUST be inlined as we can not use a stack -- CAR or real ram */
/* By yhlu 6.2005 */
/* Be warned, this function is used by both the BSP and APs to disable the
 * fixed MTRRs used for CAR.
 */
inline __attribute__((always_inline)) void disable_cache_as_ram(void)
{
	__asm__ volatile (

	/* Disable the cache while we change MTRRs */
	"	movl	%cr0, %eax	\n"
	"	orl	$(0x1<<30),%eax	\n"
	"	movl	%eax, %cr0	\n"

	/* clear sth */
	"	movl	$0x269, %ecx	\n"	/* fix4k_c8000 */
	"	xorl	%edx, %edx	\n"
	"	xorl	%eax, %eax	\n"
	"	wrmsr\n\t"
#if CONFIG_CARSIZE > 0x8000
	"	movl	$0x268, %ecx	\n"	/* fix4k_c0000 */
	"	wrmsr	\n"
#endif
	/* disable fixed mtrr for now, it will be enabled by coreboot_ram again*/
	"	movl	$0xC0010010, %ecx	\n"	/*SYSCFG_MSR */
	"	rdmsr	\n"
	"	andl	$(~(3<<18)), %eax	\n"	/*~(MtrrFixDramModEn | MtrrFixDramEn) */
	"	wrmsr	\n"

	/* Set the default memory type to UC, disable fixed MTRRs,
	 * and leave variable MTRRs enabled */
	"	movl	$0x2ff, %ecx	\n"	/* $MTRRdefType_MSR */
	"	xorl	%edx, %edx	\n"
	"	movl	$0x00000800, %eax	\n"
	"	wrmsr	\n"

	/* enable cache */
	"	movl	%cr0, %eax	\n"
	"	andl	$0x9fffffff,%eax	\n"
	"	movl	%eax, %cr0	\n"
	);
}


/**
 * Disable Cache As RAM (CAR) after memory is setup.
 *
 */
void disable_car(void)
{
	/* inlined function that disables the fixed MTRRs that
	 * are used for CAR stack space. The cache tags are still
	 * valid and the stack data is still in place. */
	disable_cache_as_ram();

	/* Now we need to get the cached data to RAM.
	 * The BKDG says that a WBINVD will not flush CAR to RAM (because the
	 * cache tags are not dirty).
	 * Solution:
	 * - Two subsequent memcpy in the same inline asm block, one from stack
	 *   to backup, one from backup to stack.
	 * The solution for Geode of using a inline asm memcpy of the stack
	 * onto itself will not mark the cache tags as dirty on K8.
	 */
	__asm__ __volatile__(
	"	movl %[carbase], %%esi		\n"
	"	movl %[backuplocation], %%edi	\n"
	"	movl %[carsizequads], %%ecx	\n"
	"	cld				\n"
	"	rep movsl			\n"
	"	wbinvd				\n"
	"	movl %[backuplocation], %%esi	\n"
	"	movl %[carbase], %%edi		\n"
	"	movl %[carsizequads], %%ecx	\n"
	"	rep movsl			\n"
	:: [carbase] "i" (CONFIG_CARBASE), [backuplocation] "i" (0x88000),
	   [carsizequads] "i" (CONFIG_CARSIZE/4)
	: "memory", "%edi", "%esi", "%ecx");
	banner(BIOS_DEBUG, "Disable_car: done wbinvd");

	/* We're now running in ram.
	 * Setup the cache for normal operation. */
	set_mtrr_ram_access();
	banner(BIOS_DEBUG, "disable_car: done");
	stage1_phase3();
}
