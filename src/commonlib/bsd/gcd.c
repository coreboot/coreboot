/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/gcd.h>
#include <commonlib/bsd/helpers.h>
#include <stdint.h>

uint32_t gcd32(uint32_t a, uint32_t b)
{
	uint32_t c;

	if (a == 0 || b == 0)
		return MAX(a, b);

	c = a % b;

	while (c > 0) {
		a = b;
		b = c;
		c = a % b;
	}

	return b;
}
