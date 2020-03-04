/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#ifndef __ARCH_BARRIER_H_
#define __ARCH_BARRIER_H__

static inline void mb(void)  { asm volatile("fence"); }
static inline void rmb(void) { asm volatile("fence"); }
static inline void wmb(void) { asm volatile("fence"); }

#endif /* __ARCH_BARRIER_H__ */
