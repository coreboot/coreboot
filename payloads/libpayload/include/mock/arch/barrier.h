/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_BARRIER_H__
#define __ARCH_BARRIER_H__

/* No memory barrier on mock build */
#define mb()
/* No read memory barrier on mock build */
#define rmb()
/* No write memory barrier on mock build */
#define wmb()

#endif /* __ARCH_BARRIER_H__ */
