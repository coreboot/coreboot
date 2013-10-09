/* Miscellaneous BPABI functions.

   Copyright (C) 2003, 2004  Free Software Foundation, Inc.
   Contributed by CodeSourcery, LLC.

   This file is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combine
   executable.)

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#if defined __GNUC__

#include <stdint.h>

uint64_t __udivmoddi4(uint64_t n, uint64_t d, uint64_t *rp);
extern int64_t __divdi3(int64_t, int64_t);
extern uint64_t __udivdi3(uint64_t, uint64_t);
extern int64_t __gnu_ldivmod_helper(int64_t, int64_t, int64_t *);
extern uint64_t __gnu_uldivmod_helper(uint64_t, uint64_t, uint64_t *);

typedef union
{
	struct {
		int32_t low;
		int32_t high;
	} s;
	int64_t ll;
} DWunion;

uint64_t
__udivmoddi4(uint64_t n, uint64_t d, uint64_t *rp)
{
	const DWunion nn = {.ll = n};
	const DWunion dd = {.ll = d};
	DWunion rr;
	uint32_t d0, d1, n0, n1, n2;
	uint32_t q0, q1;
	uint32_t b, bm;

	d0 = dd.s.low;
	d1 = dd.s.high;
	n0 = nn.s.low;
	n1 = nn.s.high;

	if (d1 == 0) {
		if (d0 > n1) {
			/* 0q = nn / 0D */
			udiv_qrnnd(q0, n0, n1, n0, d0);
			q1 = 0;
			/* Remainder in n0. */
		} else {
			/* qq = NN / 0d */
			if (d0 == 0)
				d0 = 1 / d0; /* Divide intentionally by zero. */

			udiv_qrnnd(q1, n1, 0, n1, d0);
			udiv_qrnnd(q0, n0, n1, n0, d0);

			/* Remainder in n0. */
		}

		if (rp != 0) {
			rr.s.low = n0;
			rr.s.high = 0;
			*rp = rr.ll;
		}
	} else {
		if (d1 > n1) {
			/* 00 = nn / DD */
			q0 = 0;
			q1 = 0;

			/* Remainder in n1n0. */
			if (rp != 0) {
				rr.s.low = n0;
				rr.s.high = n1;
				*rp = rr.ll;
			}
		} else {
			/* 0q = NN / dd */

			count_leading_zeros(bm, d1);
			if (bm == 0) {
				/* From (n1 >= d1) /\ (the most significant
				   bit of d1 is set), conclude (the most
				   significant bit of n1 is set) /\ (the
				   quotient digit q0 = 0 or 1).

				   This special case is necessary, not an
				   optimization. */

				/* The condition on the next line takes
				   advantage of that n1 >= d1 (true due to
				   program flow). */
				if (n1 > d1 || n0 >= d0) {
					q0 = 1;
					sub_ddmmss(n1, n0, n1, n0, d1, d0);
				} else
					q0 = 0;

				q1 = 0;

				if (rp != 0) {
					rr.s.low = n0;
					rr.s.high = n1;
					*rp = rr.ll;
				}
			} else {
				uint32_t m1, m0;
				/* Normalize.  */

				b = 32 - bm;

				d1 = (d1 << bm) | (d0 >> b);
				d0 = d0 << bm;
				n2 = n1 >> b;
				n1 = (n1 << bm) | (n0 >> b);
				n0 = n0 << bm;

				udiv_qrnnd(q0, n1, n2, n1, d1);
				umul_ppmm(m1, m0, q0, d0);

				if (m1 > n1 || (m1 == n1 && m0 > n0)) {
					q0--;
					sub_ddmmss(m1, m0, m1, m0, d1, d0);
				}

				q1 = 0;

				/* Remainder in (n1n0 - m1m0) >> bm. */
				if (rp != 0) {
					sub_ddmmss(n1, n0, n1, n0, m1, m0);
					rr.s.low = (n1 << b) | (n0 >> bm);
					rr.s.high = n1 >> bm;
					*rp = rr.ll;
				}
			}
		}
	}

	const DWunion ww = {{.low = q0, .high = q1}};
	return ww.ll;
}

int64_t
__divdi3(int64_t u, int64_t v)
{
	int32_t c = 0;
	DWunion uu = {.ll = u};
	DWunion vv = {.ll = v};
	int64_t w;

	if (uu.s.high < 0) {
		c = ~c;
		uu.ll = -uu.ll;
	}
	if (vv.s.high < 0) {
		c = ~c;
		vv.ll = -vv.ll;
	}

	w = __udivmoddi4(uu.ll, vv.ll, (uint64_t *)0);
	if (c)
		w = -w;

	return w;
}

int64_t
__gnu_ldivmod_helper (int64_t a, int64_t b, int64_t *remainder)
{
	int64_t quotient;

	quotient = __divdi3(a, b);
	*remainder = a - b * quotient;
	return quotient;
}

uint64_t
__udivdi3(uint64_t n, uint64_t d)
{
	return __udivmoddi4(n, d, (uint64_t *)0);
}

uint64_t
__gnu_uldivmod_helper(uint64_t a, uint64_t b, uint64_t *remainder)
{
	uint64_t quotient;

	quotient = __udivdi3(a, b);
	*remainder = a - b * quotient;
	return quotient;
}

#endif
