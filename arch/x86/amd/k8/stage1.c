/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 * Set the MTRR for initial ram access. 
 * be warned, this will be used by core other than core 0/node 0 or core0/node0 when cpu_reset. 
 * This warning has some significance I don't yet understand. 
 */
void set_init_ram_access(void)
{
	stage1_set_var_mtrr(0, 0x00000000, CONFIG_CBMEMK << 10, MTRR_TYPE_WRBACK);
}

/**
 * Disable Cache As RAM (CAR) after memory is setup.
 *
 * Unknown how to do this just yet. 
 */
void disable_car(void)
{
	/* call the inlined function */
	disable_cache_as_ram();

	/* The BKDG says that a WBINVD will not flush CAR to RAM (because the
	 * cache tags are not dirty).
	 * Solution:
	 * - Two subsequent memcpy in the same inline asm block, one from stack
	 *   to backup, one from backup to stack.
	 * The solution for geode of using a inline asm memcpy of the stack
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
	/* we're now running in ram. Although this will be called again, it does no harm to call it here. */
	set_init_ram_access();
	banner(BIOS_DEBUG, "disable_car: done");
	stage1_phase3();
}
