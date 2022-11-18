/* SPDX-License-Identifier: GPL-2.0-only */

#include "libgcc.h"
uint64_t __umoddi3(uint64_t num, uint64_t den)
{
	uint64_t v;
	(void)__udivmoddi4(num, den, &v);
	return v;
}
