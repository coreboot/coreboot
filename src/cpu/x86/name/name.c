/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/x86/name.h>
#include <stdint.h>
#include <string.h>

void fill_processor_name(char *processor_name)
{
	struct cpuid_result regs;
	char *processor_name_start;
	uint32_t name_as_ints[13];
	int i;

	for (i = 0; i < 3; i++) {
		regs = cpuid(0x80000002 + i);
		name_as_ints[i * 4 + 0] = regs.eax;
		name_as_ints[i * 4 + 1] = regs.ebx;
		name_as_ints[i * 4 + 2] = regs.ecx;
		name_as_ints[i * 4 + 3] = regs.edx;
	}

	name_as_ints[12] = 0;

	/* Skip leading spaces. */
	processor_name_start = (char *)name_as_ints;
	while (*processor_name_start == ' ')
		processor_name_start++;

	strcpy(processor_name, processor_name_start);
}
