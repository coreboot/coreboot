/* Copyright (c) 2012-2013, Linaro Limited
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
       * Neither the name of the Linaro nor the
         names of its contributors may be used to endorse or promote products
         derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES ; LOSS OF USE,
   DATA, OR PROFITS					   ; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* Assumptions:
 *
 * ARMv8-a, AArch64
 * Unaligned accesses
 *
 */

#define dstin		x0
#define val		w1
#define count		x2
#define tmp1		x3
#define tmp1w		w3
#define tmp2		x4
#define tmp2w		w4
#define zva_len_x	x5
#define zva_len		w5
#define zva_bits_x	x6

#define A_l		x7
#define A_lw		w7
#define dst		x8
#define tmp3w		w9

.macro def_fn f p2align=0
.text
.p2align \p2align
.global \f
.type \f, %function
\f:
.endm

def_fn memset p2align=6

	mov	dst, dstin		/* Preserve return value.  */
	ands	A_lw, val, #255
	orr	A_lw, A_lw, A_lw, lsl #8
	orr	A_lw, A_lw, A_lw, lsl #16
	orr	A_l, A_l, A_l, lsl #32
.Ltail_maybe_long:
	cmp	count, #64
	b.ge	.Lnot_short
.Ltail_maybe_tiny:
	cmp	count, #15
	b.le	.Ltail15tiny
.Ltail63:
	ands	tmp1, count, #0x30
	b.eq	.Ltail15
	add	dst, dst, tmp1
	cmp	tmp1w, #0x20
	b.eq	1f
	b.lt	2f
	stp	A_l, A_l, [dst, #-48]
1:
	stp	A_l, A_l, [dst, #-32]
2:
	stp	A_l, A_l, [dst, #-16]

.Ltail15:
	and	count, count, #15
	add	dst, dst, count
	stp	A_l, A_l, [dst, #-16]	/* Repeat some/all of last store. */
	ret

.Ltail15tiny:
	/* Set up to 15 bytes.  Does not assume earlier memory
	   being set.  */
	tbz	count, #3, 1f
	str	A_l, [dst], #8
1:
	tbz	count, #2, 1f
	str	A_lw, [dst], #4
1:
	tbz	count, #1, 1f
	strh	A_lw, [dst], #2
1:
	tbz	count, #0, 1f
	strb	A_lw, [dst]
1:
	ret

	/* Critical loop.  Start at a new cache line boundary.  Assuming
	 * 64 bytes per line, this ensures the entire loop is in one line.  */
	.p2align 6
.Lnot_short:
	neg	tmp2, dst
	ands	tmp2, tmp2, #15
	b.eq	2f
	/* Bring DST to 128-bit (16-byte) alignment.  We know that there's
	 * more than that to set, so we simply store 16 bytes and advance by
	 * the amount required to reach alignment.  */
	sub	count, count, tmp2
	stp	A_l, A_l, [dst]
	add	dst, dst, tmp2
	/* There may be less than 63 bytes to go now.  */
	cmp	count, #63
	b.le	.Ltail63
2:
	sub	dst, dst, #16		/* Pre-bias.  */
	sub	count, count, #64
1:
	stp	A_l, A_l, [dst, #16]
	stp	A_l, A_l, [dst, #32]
	stp	A_l, A_l, [dst, #48]
	stp	A_l, A_l, [dst, #64]!
	subs	count, count, #64
	b.ge	1b
	tst	count, #0x3f
	add	dst, dst, #16
	b.ne	.Ltail63
	ret
