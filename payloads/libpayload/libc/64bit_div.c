/*
 * This file is part of the libpayload project.
 *
 * Copyright 2014 Google Inc.
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

#include <libpayload-config.h>
#include <stdlib.h>

#if !IS_ENABLED(CONFIG_LP_LITTLE_ENDIAN)
#error this code is for little endian only
#endif

union overlay64 {
	uint64_t longw;
	struct {
		uint32_t lower;
		uint32_t higher;
	} words;
};


uint64_t __ashldi3(uint64_t num, unsigned shift)
{
	union overlay64 output;

	output.longw = num;
	if (shift >= 32) {
		output.words.higher = output.words.lower << (shift - 32);
		output.words.lower = 0;
	} else {
		if (!shift)
			return num;
		output.words.higher = (output.words.higher << shift) |
			(output.words.lower >> (32 - shift));
		output.words.lower = output.words.lower << shift;
	}
	return output.longw;
}

uint64_t __lshrdi3(uint64_t num, unsigned shift)
{
	union overlay64 output;

	output.longw = num;
	if (shift >= 32) {
		output.words.lower = output.words.higher >> (shift - 32);
		output.words.higher = 0;
	} else {
		if (!shift)
			return num;
		output.words.lower = output.words.lower >> shift |
			(output.words.higher << (32 - shift));
		output.words.higher = output.words.higher >> shift;
	}
	return output.longw;
}

#define MAX_32BIT_UINT ((((uint64_t)1) << 32) - 1)

static uint64_t _64bit_divide(uint64_t dividend,
			      uint64_t divider, uint64_t *rem_p)
{
	uint64_t result = 0;

	/*
	 * If divider is zero - let the rest of the system care about the
	 * exception.
	 */
	if (!divider)
		return 1/(uint32_t)divider;

	/* As an optimization, let's not use 64 bit division unless we must. */
	if (dividend <= MAX_32BIT_UINT) {
		if (divider > MAX_32BIT_UINT) {
			result = 0;
			if (rem_p)
				*rem_p = divider;
		} else {
			result = (uint32_t) dividend / (uint32_t) divider;
			if (rem_p)
				*rem_p = (uint32_t) dividend %
					(uint32_t) divider;
		}
		return result;
	}

	while (divider <= dividend) {
		uint64_t locald = divider;
		uint64_t limit = __lshrdi3(dividend, 1);
		int shifts = 0;

		while (locald <= limit) {
			shifts++;
			locald = locald + locald;
		}
		result |= __ashldi3(1, shifts);
		dividend -= locald;
	}

	if (rem_p)
		*rem_p = dividend;

	return result;
}

uint64_t __udivdi3(uint64_t num, uint64_t den)
{
	return _64bit_divide(num, den, NULL);
}

uint64_t __umoddi3(uint64_t num, uint64_t den)
{
	uint64_t v = 0;

	_64bit_divide(num, den, &v);
	return v;
}
