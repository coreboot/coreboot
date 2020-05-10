/* SPDX-License-Identifier: GPL-2.0-only */

static __always_inline void hlt(void)
{
	while (1)
		;
}
