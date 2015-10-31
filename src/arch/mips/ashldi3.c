/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google, Inc.
 *
 * Based on linux arch/mips/lib/ashldi3.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ORDER_LITTLE_ENDIAN__
#errror "What endian are you!?"
#endif

typedef unsigned word_type;
long long __ashldi3(long long u, word_type b);

struct DWstruct {
	int low, high;
};
typedef union {
	struct DWstruct s;
	long long ll;
} DWunion;

long long __ashldi3(long long u, word_type b)
{
	DWunion uu, w;
	word_type bm;

	if (b == 0)
		return u;

	uu.ll = u;
	bm = 32 - b;

	if (bm <= 0) {
		w.s.low = 0;
		w.s.high = (unsigned int) uu.s.low << -bm;
	} else {
		const unsigned int carries = (unsigned int) uu.s.low >> bm;

		w.s.low = (unsigned int) uu.s.low << b;
		w.s.high = ((unsigned int) uu.s.high << b) | carries;
	}

	return w.ll;
}
