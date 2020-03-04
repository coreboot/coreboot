/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/asm.h>

/*
 * Copy a buffer from src to dest (alignment handled by the hardware)
 *
 * Parameters:
 *	x0 - dest
 *	x1 - src
 *	x2 - n
 * Returns:
 *	x0 - dest
 */
ENTRY(memcpy)
	mov	x4, x0
	subs	x2, x2, #8
	b.mi	2f
1:	ldr	x3, [x1], #8
	subs	x2, x2, #8
	str	x3, [x4], #8
	b.pl	1b
2:	adds	x2, x2, #4
	b.mi	3f
	ldr	w3, [x1], #4
	sub	x2, x2, #4
	str	w3, [x4], #4
3:	adds	x2, x2, #2
	b.mi	4f
	ldrh	w3, [x1], #2
	sub	x2, x2, #2
	strh	w3, [x4], #2
4:	adds	x2, x2, #1
	b.mi	5f
	ldrb	w3, [x1]
	strb	w3, [x4]
5:	ret
ENDPROC(memcpy)
