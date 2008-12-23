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

/**
 * Disable Cache As RAM (CAR) after memory is setup.
 */
void disable_car(void)
{
	printk(BIOS_DEBUG, "disable_car entry\n");
	/* Determine new global variable location. Stack organization from top
	 * Top 4 bytes are reserved
	 * Pointer to global variables
	 * Global variables
	 *
 	 * Align the result to 8 bytes
	 */
	struct global_vars *const newlocation = (struct global_vars *)((RAM_STACK_BASE - sizeof(struct global_vars *) - sizeof(struct global_vars)) & ~0x7);
	/* Copy global variables to new location. */
	memcpy(newlocation, global_vars(), sizeof(struct global_vars));
	printk(BIOS_DEBUG, "disable_car global_vars copy done\n");
	/* Set the new global variable pointer. */
	*(struct global_vars **)(RAM_STACK_BASE - sizeof(struct global_vars *)) = newlocation;

	printk(BIOS_DEBUG, "disable_car global_vars pointer adjusted\n");
	printk(BIOS_DEBUG, "entering asm code now\n");

	__asm__ __volatile__(
	"	movl	%[newesp], %%esp	\n"

	/* We don't need cache as ram for now on */
	/* disable cache */
	"	movl    %%cr0, %%eax		\n"
	"	orl    $(0x1<<30),%%eax		\n"
	"	movl    %%eax, %%cr0		\n"

	/* disable fixed mtrr from now on, it will be enabled by coreboot_ram again*/
	/* clear sth */
	"	xorl    %%eax, %%eax		\n"
	"	xorl    %%edx, %%edx		\n"
	"	movl    $0x201, %%ecx		\n"
	"	wrmsr				\n"
	"	movl    $0x200, %%ecx		\n"
	"	wrmsr				\n"

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

	"	invd				\n"

	"	call stage1_phase3		\n"
	:: [newesp] "i" (newlocation),
	 [_MTRRdefType_MSR] "i" (MTRRdefType_MSR)
	: "memory");
}

void stop_ap(void)
{
}

