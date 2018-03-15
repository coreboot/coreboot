/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <stddef.h>
#include <stdint.h>
#include <cpu/x86/smm.h>

/*
 * calls into SMM with the given cmd and subcmd in eax, and arg in ebx
 *
 * static inline because the resulting assembly is often smaller than
 * the call sequence due to constant folding.
 */
static inline u32 call_smm(u8 cmd, u8 subcmd, void *arg)
{
	u32 res = 0;
	__asm__ __volatile__ (
		"outb %b0, %3"
		: "=a" (res)
		: "a" ((subcmd << 8) | cmd), "b" (arg), "i" (APM_CNT));
	return res;
}
