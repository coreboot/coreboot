/* SPDX-License-Identifier: GPL-2.0-only */

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
