/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <smp/node.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic_def.h>

#if CONFIG(SMP)
int boot_cpu(void)
{
	int bsp;
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	bsp = !!(msr.lo & (1 << 8));
	return bsp;
}
#endif
