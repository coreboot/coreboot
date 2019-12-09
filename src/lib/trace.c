/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Rudolf Marek <r.marek@assembler.cz>
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

#include <types.h>
#include <console/console.h>
#include <trace.h>

int volatile trace_dis = 0;

void __cyg_profile_func_enter(void *func, void *callsite)
{

	if (trace_dis)
		return;

	DISABLE_TRACE
	printk(BIOS_INFO, "~%p(%p)\n", func, callsite);
	ENABLE_TRACE
}

void __cyg_profile_func_exit(void *func, void *callsite)
{
}
