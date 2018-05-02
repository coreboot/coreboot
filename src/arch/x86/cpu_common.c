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

#include <console/console.h>
#include <cpu/cpu.h>
#include <arch/io.h>
#include <string.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <device/path.h>
#include <device/device.h>
#include <smp/spinlock.h>

#ifndef __x86_64__
/* Standard macro to see if a specific flag is changeable */
static inline int flag_is_changeable_p(uint32_t flag)
{
	uint32_t f1, f2;

	asm(
		"pushfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"movl %0,%1\n\t"
		"xorl %2,%0\n\t"
		"pushl %0\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"popfl\n\t"
		: "=&r" (f1), "=&r" (f2)
		: "ir" (flag));
	return ((f1^f2) & flag) != 0;
}

/* Probe for the CPUID instruction */
int cpu_have_cpuid(void)
{
	return flag_is_changeable_p(X86_EFLAGS_ID);
}

#else

int cpu_have_cpuid(void)
{
	return 1;
}
#endif

int cpu_cpuid_extended_level(void)
{
	return cpuid_eax(0x80000000);
}

int cpu_phys_address_size(void)
{
	if (!(cpu_have_cpuid()))
		return 32;

	if (cpu_cpuid_extended_level() >= 0x80000008)
		return cpuid_eax(0x80000008) & 0xff;

	if (cpuid_edx(1) & (CPUID_FEATURE_PAE | CPUID_FEATURE_PSE36))
		return 36;
	return 32;
}
