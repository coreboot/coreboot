/*
 *
 * Copyright (C) 2020 Google, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>

/*
 * This file implements operations for a simple 32.32 fixed-point math type.
 * This is intended for speed-critical stuff (e.g. graphics) so there are
 * intentionally no overflow checks or assertions, and operations are written
 * to prefer speed over precision (e.g. multiplying by 1 may lose precision).
 * For best results, only use for applications where 16.16 would fit.
 */

typedef struct {		/* wrap in struct to prevent direct access */
	int64_t v;
} fpmath_t;

#define FPMATH_SHIFT 32		/* define where to place the decimal point */

/* Turn an integer into an fpmath_t. */
static inline fpmath_t fp(int32_t a)
{
	return (fpmath_t){ .v = (int64_t)a << FPMATH_SHIFT };
}

/* Create an fpmath_t from a fraction. (numerator / denominator)  */
static inline fpmath_t fpfrac(int32_t numerator, int32_t denominator)
{
	return (fpmath_t){ .v = ((int64_t)numerator << FPMATH_SHIFT) / denominator };
}

/* Turn an fpmath_t back into an integer, rounding towards -INF. */
static inline int32_t fpfloor(fpmath_t a)
{
	return a.v >> FPMATH_SHIFT;
}

/* Turn an fpmath_t back into an integer, rounding towards nearest. */
static inline int32_t fpround(fpmath_t a)
{
	return (a.v + ((int64_t)1 << (FPMATH_SHIFT - 1))) >> FPMATH_SHIFT;
}

/* Turn an fpmath_t back into an integer, rounding towards +INF. */
static inline int32_t fpceil(fpmath_t a)
{
	return (a.v + ((int64_t)1 << FPMATH_SHIFT) - 1) >> FPMATH_SHIFT;
}

/* Add two fpmath_t. (a + b) */
static inline fpmath_t fpadd(fpmath_t a, fpmath_t b)
{
	return (fpmath_t){ .v = a.v + b.v };
}

/* Add an fpmath_t and an integer. (a + b) */
static inline fpmath_t fpaddi(fpmath_t a, int32_t b)
{
	return (fpmath_t){ .v = a.v + ((int64_t)b << FPMATH_SHIFT) };
}

/* Subtract one fpmath_t from another. (a + b) */
static inline fpmath_t fpsub(fpmath_t a, fpmath_t b)
{
	return (fpmath_t){ .v = a.v - b.v };
}

/* Subtract an integer from an fpmath_t. (a - b) */
static inline fpmath_t fpsubi(fpmath_t a, int32_t b)
{
	return (fpmath_t){ .v = a.v - ((int64_t)b << FPMATH_SHIFT) };
}

/* Subtract an fpmath_t from an integer. (a - b) */
static inline fpmath_t fpisub(int32_t a, fpmath_t b)
{
	return (fpmath_t){ .v = ((int64_t)a << FPMATH_SHIFT) - b.v };
}

/* Multiply two fpmath_t. (a * b)
   Looses 16 bits fractional precision on each. */
static inline fpmath_t fpmul(fpmath_t a, fpmath_t b)
{
	return (fpmath_t){ .v = (a.v >> (FPMATH_SHIFT/2)) * (b.v >> (FPMATH_SHIFT/2)) };
}

/* Multiply an fpmath_t and an integer. (a * b) */
static inline fpmath_t fpmuli(fpmath_t a, int32_t b)
{
	return (fpmath_t){ .v = a.v * b };
}

/* Divide an fpmath_t by another. (a / b)
   Truncates integral part of a to 16 bits! Careful with this one! */
static inline fpmath_t fpdiv(fpmath_t a, fpmath_t b)
{
	return (fpmath_t){ .v = (a.v << (FPMATH_SHIFT/2)) / (b.v >> (FPMATH_SHIFT/2)) };
}

/* Divide an fpmath_t by an integer. (a / b) */
static inline fpmath_t fpdivi(fpmath_t a, int32_t b)
{
	return (fpmath_t){ .v = a.v / b };
}

/* Calculate absolute value of an fpmath_t. (ABS(a)) */
static inline fpmath_t fpabs(fpmath_t a)
{
	return (fpmath_t){ .v = (a.v < 0 ? -a.v : a.v) };
}

/* Return true iff two fpmath_t are exactly equal. (a == b)
   Like with floats, you probably don't want to use this most of the time. */
static inline int fpequals(fpmath_t a, fpmath_t b)
{
	return a.v == b.v;
}

/* Return true iff one fpmath_t is less than another. (a < b) */
static inline int fpless(fpmath_t a, fpmath_t b)
{
	return a.v < b.v;
}

/* Return true iff one fpmath_t is more than another. (a > b) */
static inline int fpmore(fpmath_t a, fpmath_t b)
{
	return a.v > b.v;
}

/* Return the smaller of two fpmath_t. (MIN(a, b)) */
static inline fpmath_t fpmin(fpmath_t a, fpmath_t b)
{
	if (a.v < b.v)
		return a;
	else
		return b;
}

/* Return the larger of two fpmath_t. (MAX(a, b)) */
static inline fpmath_t fpmax(fpmath_t a, fpmath_t b)
{
	if (a.v > b.v)
		return a;
	else
		return b;
}

/* Return the constant PI as an fpmath_t. */
static inline fpmath_t fppi(void)
{
	/* Rounded (uint64_t)(M_PI * (1UL << 60)) to nine hex digits. */
	return (fpmath_t){ .v = 0x3243f6a89 };
}

/*
 * Returns the "one-based" sine of an fpmath_t, meaning the input is interpreted as if the range
 * 0.0-1.0 corresponded to 0.0-PI/2 for radians. This is mostly here as the base primitives for
 * the other trig stuff, but it may be useful to use directly if your input value already needs
 * to be multiplied by some factor of PI and you want to save the instructions (and precision)
 * for multiplying it in just so that the trig functions can divide it right out again.
 */
fpmath_t fpsin1(fpmath_t x);

/* Returns the "one-based" cosine of an fpmath_t (analogous definition to fpsin1()). */
static inline fpmath_t fpcos1(fpmath_t x)
{
	return fpsin1(fpaddi(x, 1));
}

/* Returns the sine of an fpmath_t interpreted as radians. */
static inline fpmath_t fpsinr(fpmath_t radians)
{
	return fpsin1(fpdiv(radians, fpdivi(fppi(), 2)));
}

/* Returns the sine of an fpmath_t interpreted as degrees. */
static inline fpmath_t fpsind(fpmath_t degrees)
{
	return fpsin1(fpdivi(degrees, 90));
}

/* Returns the cosine of an fpmath_t interpreted as radians. */
static inline fpmath_t fpcosr(fpmath_t radians)
{
	return fpcos1(fpdiv(radians, fpdivi(fppi(), 2)));
}

/* Returns the cosine of an fpmath_t interpreted as degrees. */
static inline fpmath_t fpcosd(fpmath_t degrees)
{
	return fpcos1(fpdivi(degrees, 90));
}

/* Returns the tangent of an fpmath_t interpreted as radians.
   No guard rails, don't call this at the poles or you'll divide by 0! */
static inline fpmath_t fptanr(fpmath_t radians)
{
	fpmath_t one_based = fpdiv(radians, fpdivi(fppi(), 2));
	return fpdiv(fpsin1(one_based), fpcos1(one_based));
}

/* Returns the tangent of an fpmath_t interpreted as degrees.
   No guard rails, don't call this at the poles or you'll divide by 0! */
static inline fpmath_t fptand(fpmath_t degrees)
{
	fpmath_t one_based = fpdivi(degrees, 90);
	return fpdiv(fpsin1(one_based), fpcos1(one_based));
}
