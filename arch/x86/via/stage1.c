/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Carl-Daniel Hailfinger
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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <msr.h>
#include <macros.h>
#include <cpu.h>
#include <stage1.h>
#include <globalvars.h>
#include <string.h>
#include <mtrr.h>
#include <via_c7.h>

#ifdef NO_IDEA_WHETHER_THIS_IS_RELEVANT_ON_C7
/**
 * Set the MTRR for initial ram access. 
 * be warned, this will be used by core other than core 0/node 0 or core0/node0 when cpu_reset. 
 * This warning has some significance I don't yet understand. 
 */
void set_init_ram_access(void)
{
	set_var_mtrr(0, 0x00000000, CONFIG_CBMEMK << 10, MTRR_TYPE_WRBACK);
}
#endif

#define __stringify_1(x)	#x
#define __stringify(x)		__stringify_1(x)

/**
 * Disable Cache As RAM (CAR) after memory is setup.
 */
void disable_car(void)
{
	/* Determine new global variable location. Stack organization from top
	 * Top 4 bytes are reserved
	 * Pointer to global variables
	 * Global variables
	 *
 	 * Align the result to 8 bytes
	 */
	const struct global_vars *newlocation = (struct global_vars *)((RAM_STACK_BASE - sizeof(struct global_vars *) - sizeof(struct global_vars)) & ~0x7);
	/* Copy global variables to new location. */
	memcpy(newlocation, global_vars(), sizeof(struct global_vars));
	/* Set the new global variable pointer. */
	*(struct global_vars **)(RAM_STACK_BASE - sizeof(struct global_vars *)) = newlocation;

	__asm__ __volatile__(
	/* We don't need cache as ram for now on */
	/* disable cache */
	"	movl    %%cr0, %%eax		\n"
	"	orl    $(0x1<<30),%%eax		\n"
	"	movl    %%eax, %%cr0		\n"

	/* disable fixed mtrr from now on, it will be enabled by coreboot_ram again*/
	"	movl    %[_SYSCFG_MSR], %%ecx	\n"
	"	rdmsr				\n"
	"	andl    %[_SYSCFG_MSR_newval], %%eax\n"
//	"	andl    $(~(SYSCFG_MSR_MtrrFixDramModEn | SYSCFG_MSR_MtrrFixDramEn)), %%eax\n"
	/* clear sth */
	"	wrmsr				\n"
#warning Must clear MTRR 0x200 and 0x201

	/* Set the default memory type and disable fixed and enable variable MTRRs */
	"	movl    %[_MTRRdefType_MSR], %%ecx	\n"
	"	xorl    %%edx, %%edx		\n"
	/* Enable Variable and Disable Fixed MTRRs */
	"	movl    $0x00000800, %%eax	\n"
	"	wrmsr				\n"

	/* enable cache */
	"	movl    %%cr0, %%eax		\n"
	"	andl    $0x9fffffff,%%eax	\n"
	"	movl    %%eax, %%cr0		\n"

	"	wbinvd				\n"

	"	movl	%[newesp], %%esp	\n"
	"	call stage1_phase3		\n"
	:: [newesp] "i" (newlocation),
	 [_SYSCFG_MSR] "i" (SYSCFG_MSR),
	 [_SYSCFG_MSR_newval] "i" (~(SYSCFG_MSR_MtrrFixDramModEn | SYSCFG_MSR_MtrrFixDramEn)),
	 [_SYSCFG_MSR_MtrrFixDramModEn] "i" (SYSCFG_MSR_MtrrFixDramModEn),
	 [_SYSCFG_MSR_MtrrFixDramEn] "i" (SYSCFG_MSR_MtrrFixDramEn),
	 [_MTRRdefType_MSR] "i" (MTRRdefType_MSR)
	: "memory");
}
