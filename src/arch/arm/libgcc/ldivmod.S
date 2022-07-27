/* SPDX-License-Identifier: ISC OR GPL-2.0-only */

/*
 * ldivmod.S: signed 64 bit division (quotient and remainder)
 *
 * Taken from CrOS EC: third_party/libaeabi-cortexm0/core/cortex-m0/ldivmod.S
 */

#include <arch/asm.h>


@ {long long quotient, long long remainder}
@ __aeabi_ldivmod(long long numerator, long long denominator)
@
@ Divide r1:r0 by r3:r2 and return the quotient in r1:r0 and the remainder in
@ r3:r2 (all signed)
@

ENTRY(__aeabi_ldivmod)

	cmp	r1, #0
	bge	L_num_pos

	push	{r4, lr}
	movs	r4, #0			@ num = -num
	rsbs	r0, r0, #0
	sbcs	r4, r1
	mov	r1, r4

	cmp	r3, #0
	bge	L_neg_both

	movs	r4, #0			@ den = -den
	rsbs	r2, r2, #0
	sbcs	r4, r3
	mov	r3, r4
	bl	__aeabi_uldivmod
	movs	r4, #0			@ rem = -rem
	rsbs	r2, r2, #0
	sbcs	r4, r3
	mov	r3, r4
	pop	{r4, pc}

L_neg_both:
	bl	__aeabi_uldivmod
	movs	r4, #0			@ quot = -quot
	rsbs	r0, r0, #0
	sbcs	r4, r1
	mov	r1, r4
	movs	r4, #0			@ rem = -rem
	rsbs	r2, r2, #0
	sbcs	r4, r3
	mov	r3, r4
	pop	{r4, pc}

L_num_pos:
	cmp	r3, #0
	blt	L_den_neg
	push	{r4, lr}
	bl	__aeabi_uldivmod	@ offset too big for b / bge
	pop	{r4, pc}

L_den_neg:
	push	{r4, lr}
	movs	r4, #0			@ den = -den
	rsbs	r2, r2, #0
	sbcs	r4, r3
	mov	r3, r4
	bl	__aeabi_uldivmod
	movs	r4, #0			@ quot = -quot
	rsbs	r0, r0, #0
	sbcs	r4, r1
	mov	r1, r4
	pop	{r4, pc}
