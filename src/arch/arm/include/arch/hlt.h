/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef ARCH_HLT_H
#define ARCH_HLT_H

static __always_inline void hlt(void)
{
	for (;;) ;
}

#endif /* ARCH_HLT_H */
