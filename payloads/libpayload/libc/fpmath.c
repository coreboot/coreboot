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

#include <fpmath.h>

/*
 * This table represents one ascending arc of the sine curve, i.e. the values sin(x) for
 * 0.0 <= x < PI/2. We divide that range into 256 equidistant points and store the corresponding
 * sine values for those points. Since the values lie in the range 0.0 <= sin(x) < 1.0, in order
 * to make the most use of the bytes we store, we map them to the range from 0 to 2^16.
 *
 * Generated with:
 *
 * for (i = 0; i < 256; i++) {
 *     double s = sin((double)i * M_PI / 2 / 256);
 *     uint16_t u = fmin(round(s * (1 << 16)), (1 << 16));
 *     printf("0x%04x,%s", u, i % 8 == 7 ? "\n" : " ");
 * }
 *
 * In order to make sure the second access for linear interpolation (see below) cannot overrun
 * the array, we stick a final 257th value 0xffff at the end. (It should really be 0x10000,
 * but... this is good enough.)
 */

/* Table size as power of two. If we ever want to change the table size, updating this value
   should make everything else fall back into place again (hopefully). */
#define TP2 8

static const uint16_t fpsin_table[(1 << TP2) + 1] = {
	0x0000, 0x0192, 0x0324, 0x04b6, 0x0648, 0x07da, 0x096c, 0x0afe,
	0x0c90, 0x0e21, 0x0fb3, 0x1144, 0x12d5, 0x1466, 0x15f7, 0x1787,
	0x1918, 0x1aa8, 0x1c38, 0x1dc7, 0x1f56, 0x20e5, 0x2274, 0x2402,
	0x2590, 0x271e, 0x28ab, 0x2a38, 0x2bc4, 0x2d50, 0x2edc, 0x3067,
	0x31f1, 0x337c, 0x3505, 0x368e, 0x3817, 0x399f, 0x3b27, 0x3cae,
	0x3e34, 0x3fba, 0x413f, 0x42c3, 0x4447, 0x45cb, 0x474d, 0x48cf,
	0x4a50, 0x4bd1, 0x4d50, 0x4ecf, 0x504d, 0x51cb, 0x5348, 0x54c3,
	0x563e, 0x57b9, 0x5932, 0x5aaa, 0x5c22, 0x5d99, 0x5f0f, 0x6084,
	0x61f8, 0x636b, 0x64dd, 0x664e, 0x67be, 0x692d, 0x6a9b, 0x6c08,
	0x6d74, 0x6edf, 0x7049, 0x71b2, 0x731a, 0x7480, 0x75e6, 0x774a,
	0x78ad, 0x7a10, 0x7b70, 0x7cd0, 0x7e2f, 0x7f8c, 0x80e8, 0x8243,
	0x839c, 0x84f5, 0x864c, 0x87a1, 0x88f6, 0x8a49, 0x8b9a, 0x8ceb,
	0x8e3a, 0x8f88, 0x90d4, 0x921f, 0x9368, 0x94b0, 0x95f7, 0x973c,
	0x9880, 0x99c2, 0x9b03, 0x9c42, 0x9d80, 0x9ebc, 0x9ff7, 0xa130,
	0xa268, 0xa39e, 0xa4d2, 0xa605, 0xa736, 0xa866, 0xa994, 0xaac1,
	0xabeb, 0xad14, 0xae3c, 0xaf62, 0xb086, 0xb1a8, 0xb2c9, 0xb3e8,
	0xb505, 0xb620, 0xb73a, 0xb852, 0xb968, 0xba7d, 0xbb8f, 0xbca0,
	0xbdaf, 0xbebc, 0xbfc7, 0xc0d1, 0xc1d8, 0xc2de, 0xc3e2, 0xc4e4,
	0xc5e4, 0xc6e2, 0xc7de, 0xc8d9, 0xc9d1, 0xcac7, 0xcbbc, 0xccae,
	0xcd9f, 0xce8e, 0xcf7a, 0xd065, 0xd14d, 0xd234, 0xd318, 0xd3fb,
	0xd4db, 0xd5ba, 0xd696, 0xd770, 0xd848, 0xd91e, 0xd9f2, 0xdac4,
	0xdb94, 0xdc62, 0xdd2d, 0xddf7, 0xdebe, 0xdf83, 0xe046, 0xe107,
	0xe1c6, 0xe282, 0xe33c, 0xe3f4, 0xe4aa, 0xe55e, 0xe610, 0xe6bf,
	0xe76c, 0xe817, 0xe8bf, 0xe966, 0xea0a, 0xeaab, 0xeb4b, 0xebe8,
	0xec83, 0xed1c, 0xedb3, 0xee47, 0xeed9, 0xef68, 0xeff5, 0xf080,
	0xf109, 0xf18f, 0xf213, 0xf295, 0xf314, 0xf391, 0xf40c, 0xf484,
	0xf4fa, 0xf56e, 0xf5df, 0xf64e, 0xf6ba, 0xf724, 0xf78c, 0xf7f1,
	0xf854, 0xf8b4, 0xf913, 0xf96e, 0xf9c8, 0xfa1f, 0xfa73, 0xfac5,
	0xfb15, 0xfb62, 0xfbad, 0xfbf5, 0xfc3b, 0xfc7f, 0xfcc0, 0xfcfe,
	0xfd3b, 0xfd74, 0xfdac, 0xfde1, 0xfe13, 0xfe43, 0xfe71, 0xfe9c,
	0xfec4, 0xfeeb, 0xff0e, 0xff30, 0xff4e, 0xff6b, 0xff85, 0xff9c,
	0xffb1, 0xffc4, 0xffd4, 0xffe1, 0xffec, 0xfff5, 0xfffb, 0xffff,
	0xffff,
};

/* x is in the "one-based" scale, so x == 1.0 is the top of the curve (PI/2 in radians). */
fpmath_t fpsin1(fpmath_t x)
{
	/*
	 * When doing things like sin(x)/x, tiny errors can quickly become big problems, so just
	 * returning the nearest table value we have is not good enough (our fpmath_t has four
	 * times as much fractional precision as the sine table). A good and fast enough remedy
	 * is to linearly interpolate between the two nearest table values v1 and v2.
	 * (There are better but slower interpolations so we intentionally choose this one.)
	 *
	 * Most of this math can be done in 32 bits (because we're just operating on fractional
	 * parts in the 0.0-1.0 range anyway), so to help our 32-bit platforms a bit we keep it
	 * there as long as possible and only go back to an int64_t at the end.
	 */
	uint32_t v1, v2;

	/*
	 * Since x is "one-based" the part that maps to our curve (0.0 to PI/2 in radians) just
	 * happens to be exactly the fractional part of the fpmath_t, easy to extract.
	 */
	int index = (x.v >> (FPMATH_SHIFT - TP2)) & ((1 << TP2) - 1);

	/*
	 * In our one-based input domain, the period of the sine function is exactly 4.0. By
	 * extracting the first bit of the integral part of the fpmath_t we can check if it is
	 * odd-numbered (1.0-2.0, 3.0-4.0, etc.) or even numbered (0.0-1.0, 2.0-3.0, etc.), and
	 * that tells us whether we are in a "rising" (away from 0) or "falling" (towards 0)
	 * part of the sine curve. Our table curve is rising, so for the falling parts we have
	 * to mirror the curve horizontally by using the complement of our input index.
	 */
	if (x.v & ((int64_t)1 << FPMATH_SHIFT)) {
		v1 = fpsin_table[(1 << TP2) - index];
		v2 = fpsin_table[(1 << TP2) - index - 1];
	} else {
		v1 = fpsin_table[index];
		v2 = fpsin_table[index + 1];
	}

	/*
	 * Linear interpolation: sin(x) is interpolated as the closest number sin(x0) to the
	 * left of x we have in our table, plus the distance of that value to the closest number
	 * to the right of x (sin(x1)) times the fractional distance of x to x0. Since the table
	 * is conveniently exactly 256 values, x0 is exactly the upper 8 bits of the fractional
	 * part of x, meaning all fractional bits below that represent exactly the distance we
	 * need to interpolate over. (There are 24 of them but we need to multiply them with
	 * 16-bit table values to fit exactly 32 bits, so we discard the lowest 8 bits.)
	 */
	uint32_t val = (v1 << (FPMATH_SHIFT - 16)) +
		    (v2 - v1) * ((x.v >> (16 - TP2)) & 0xffff);

	/*
	 * Just like the first integral bit told us whether we need to mirror horizontally, the
	 * second can tell us to mirror vertically. In 2.0-4.0, 6.0-8.0, etc. of the input range
	 * the sine is negative, and in 0.0-2.0, 4.0-6.0, etc. it is positive.
	 */
	if (x.v & ((int64_t)2 << FPMATH_SHIFT))
		return (fpmath_t){ .v = -(int64_t)val };
	else
		return (fpmath_t){ .v = val };
}
