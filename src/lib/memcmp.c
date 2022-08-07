/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>

int memcmp(const void *src1, const void *src2, size_t bytes)
{
	const unsigned char *s1, *s2;
	int result;
	s1 = src1;
	s2 = src2;
	result = 0;
	while ((bytes > 0) && (result == 0)) {
		result = *s1 - *s2;
		bytes--;
		s1++;
		s2++;
	}
	return result;
}
