/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_HLT_H
#define ARCH_HLT_H

static __always_inline void hlt(void)
{
	while (1);
}

#endif /* ARCH_HLT_H */
