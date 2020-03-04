/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
/* Based on linux/arch/arm/lib/ucmpdi2.S */

#if defined __GNUC__

#include <arch/asm.h>

#ifdef __ARMEB__
#define xh r0
#define xl r1
#define yh r2
#define yl r3
#else
#define xl r0
#define xh r1
#define yl r2
#define yh r3
#endif

ENTRY(__ucmpdi2)

	cmp	xh, yh
	cmpeq	xl, yl
	movlo	r0, #0
	moveq	r0, #1
	movhi	r0, #2
	mov	pc, lr

ENDPROC(__ucmpdi2)

ENTRY(__aeabi_ulcmp)

	cmp	xh, yh
	cmpeq	xl, yl
	movlo	r0, #-1
	moveq	r0, #0
	movhi	r0, #1
	mov	pc, lr

ENDPROC(__aeabi_ulcmp)

#endif
