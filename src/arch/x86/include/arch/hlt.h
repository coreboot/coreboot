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

#ifndef ARCH_HLT_H
#define ARCH_HLT_H

#if defined(__ROMCC__)
static void hlt(void)
{
	__builtin_hlt();
}
#else
#include <compiler.h>

static __always_inline void hlt(void)
{
	asm("hlt");
}
#endif

#endif /* ARCH_HLT_H */
