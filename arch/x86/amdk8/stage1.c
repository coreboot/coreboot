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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <msr.h>
#include <amd_k8.h>

/**
 * Disable Cache As RAM (CAR) after memory is setup.
 *
 * Unknown how to do this just yet. 
 */
void disable_car(void)
{
	/* OK, here is the theory: we should be able to copy 
	 * the data back over itself, and the wbinvd should then 
	 * flush to memory. Let's see. 
	 */
	__asm__ __volatile__("cld; rep movsl" ::"D" (DCACHE_RAM_BASE), "S" (DCACHE_RAM_BASE), "c" (DCACHE_RAM_SIZE/4): "memory");
	__asm__ __volatile__ ("wbinvd\n");
	banner(BIOS_DEBUG, "Disable_car: done wbinvd");
	banner(BIOS_DEBUG, "disable_car: done");
}
