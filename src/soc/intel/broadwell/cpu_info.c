/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/systemagent.h>

u32 cpu_family_model(void)
{
	return cpuid_eax(1) & 0x0fff0ff0;
}

u32 cpu_stepping(void)
{
	return cpuid_eax(1) & 0xf;
}

/* Dynamically determine if the part is ULT. */
int cpu_is_ult(void)
{
	static int ult = -1;

	if (ult < 0) {
		u32 fm = cpu_family_model();
		if (fm == BROADWELL_FAMILY_ULT || fm == HASWELL_FAMILY_ULT)
			ult = 1;
		else
			ult = 0;
	}

	return ult;
}
