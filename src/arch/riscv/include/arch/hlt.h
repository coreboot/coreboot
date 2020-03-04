/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

static __always_inline void hlt(void)
{
	while (1);
}
