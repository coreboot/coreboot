/*
 *
 * Copyright 2015 Google Inc.
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
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
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

#include <libpayload.h>

/*
 * Provide platform-independent backend implementation for __builtin_clz() in
 * <libpayload.h> in case GCC does not have an assembly version for this arch.
 */

int __clzsi2(u32 a);
int __clzsi2(u32 a)
{
	static const u8 four_bit_table[] = {
		[0x0] = 4, [0x1] = 3, [0x2] = 2, [0x3] = 2,
		[0x4] = 1, [0x5] = 1, [0x6] = 1, [0x7] = 1,
		[0x8] = 0, [0x9] = 0, [0xa] = 0, [0xb] = 0,
		[0xc] = 0, [0xd] = 0, [0xe] = 0, [0xf] = 0,
	};
	int r = 0;

	if (!(a & (0xffff << 16))) {
		r += 16;
		a <<= 16;
	}

	if (!(a & (0xff << 24))) {
		r += 8;
		a <<= 8;
	}

	if (!(a & (0xf << 28))) {
		r += 4;
		a <<= 4;
	}

	return r + four_bit_table[a >> 28];
}
