/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <string.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/name.h>

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
