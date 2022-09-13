/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <amdblocks/cpu.h>

int get_cpu_count(void)
{
	return 1 + (cpuid_ecx(0x80000008) & 0xff);
}
