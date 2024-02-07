/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/gcd.h>
#include <commonlib/bsd/helpers.h>
#include <stdint.h>

uint64_t gcd(uint64_t a, uint64_t b)
{
	uint64_t c;

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
